#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "mumps.h"
#define CHUNK_MAX_SIZE 32000
#define ROW_MAX_SIZE 1000
#ifndef MAX_STR_LEN
#define MAX_STR_LEN 256
#endif
static sqlite3 *global_sqlite_db = NULL;
extern short ST_Set(mvar *var, cstring *val);
sqlite3* get_sqlite_connection() {
    if (global_sqlite_db != NULL) {
        return global_sqlite_db;
    }
    int rc = sqlite3_open("local_mumps.db", &global_sqlite_db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, " [SQL Gateway Error] Cannot open SQLite database: %s\n", sqlite3_errmsg(global_sqlite_db));
        global_sqlite_db = NULL;
        return NULL;
    }
    printf(" [SQL Gateway] Connection to SQLite [local_mumps.db] established successfully.\n");
    return global_sqlite_db;
}
void set_mumps_sql_error(const char *error_msg) {
    mvar err_var;
    cstring err_value;
    memset(&err_var, 0, sizeof(mvar));
    err_var.uci = UCI_IS_LOCALVAR;
    err_var.slen = 0;
    snprintf((char *)err_var.name.var_cu, sizeof(err_var.name.var_cu), "%%SQLERR");
    snprintf((char *)err_value.buf, MAX_STR_LEN, "%s", error_msg);
    err_value.len = strlen((char *)err_value.buf);
    ST_Set(&err_var, &err_value);
}
void set_mumps_row_count(int total_rows) {
    mvar cnt_var;
    cstring cnt_value;
    memset(&cnt_var, 0, sizeof(mvar));
    cnt_var.uci = UCI_IS_LOCALVAR;
    cnt_var.slen = 0;
    snprintf((char *)cnt_var.name.var_cu, sizeof(cnt_var.name.var_cu), "SQLRCNT");
    snprintf((char *)cnt_value.buf, MAX_STR_LEN, "%d", total_rows);
    cnt_value.len = strlen((char *)cnt_value.buf);
    ST_Set(&cnt_var, &cnt_value);
}
int execute_sqlite_query(const char *sql_text, const char *res_prefix) {
    sqlite3 *db = get_sqlite_connection();
    if (!db) return -1;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql_text, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        char err_buf[MAX_STR_LEN];
        snprintf(err_buf, sizeof(err_buf), "[SQLite Prepare Error] %s", sqlite3_errmsg(db));
        set_mumps_sql_error(err_buf);
        return -1;
    }
    int num_cols = sqlite3_column_count(stmt);
    char cols_buffer[1024] = "";
    for (int i = 0; i < num_cols; i++) {
        const char *col_name = sqlite3_column_name(stmt, i);
        if (i > 0) strcat(cols_buffer, ",");
        strcat(cols_buffer, col_name);
    }
    mvar var_name;
    cstring val_string;
    memset(&var_name, 0, sizeof(mvar));
    var_name.uci = UCI_IS_LOCALVAR;
    var_name.slen = 0;
    snprintf((char *)var_name.name.var_cu, sizeof(var_name.name.var_cu), "SQLCOLS");
    memset(&val_string, 0, sizeof(cstring));
    val_string.len = strlen(cols_buffer);
    if (val_string.len >= MAX_STR_LEN) val_string.len = MAX_STR_LEN - 1;
    memcpy(val_string.buf, cols_buffer, val_string.len);
    val_string.buf[val_string.len] = '\0';
    ST_Set(&var_name, &val_string);
    char *chunk_buffer = (char *)malloc(CHUNK_MAX_SIZE + ROW_MAX_SIZE);
    if (!chunk_buffer) {
        sqlite3_finalize(stmt);
        return -1;
    }
    chunk_buffer[0] = '\0';
    int chunk_len = 0;
    int chunk_count = 0;
    int total_rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        char row_buffer[ROW_MAX_SIZE];
        int row_offset = 0;
        row_buffer[0] = '\0';
        for (int i = 0; i < num_cols; i++) {
            const unsigned char *col_text = sqlite3_column_text(stmt, i);
            const char *val = col_text ? (const char *)col_text : "NULL";
            if (i > 0) {
                row_offset += snprintf(row_buffer + row_offset, ROW_MAX_SIZE - row_offset, "|");
            }
            row_offset += snprintf(row_buffer + row_offset, ROW_MAX_SIZE - row_offset, "%s", val);
        }
        if (chunk_len > 0 && (chunk_len + row_offset + 1 > CHUNK_MAX_SIZE)) {
            chunk_count++;
            mvar local_var;
            cstring var_value;
            memset(&local_var, 0, sizeof(mvar));
            local_var.uci = UCI_IS_LOCALVAR;
            local_var.slen = 0;
            snprintf((char *)local_var.name.var_cu, sizeof(local_var.name.var_cu), "%c%d", res_prefix[0], chunk_count);
            memcpy(var_value.buf, chunk_buffer, chunk_len);
            var_value.len = chunk_len;
            ST_Set(&local_var, &var_value);
            chunk_buffer[0] = '\0';
            chunk_len = 0;
        }
        if (chunk_len > 0) {
            chunk_buffer[chunk_len] = '\0';
            chunk_len++;
        }
        memcpy(chunk_buffer + chunk_len, row_buffer, row_offset);
        chunk_len += row_offset;
        total_rows++;
    }
    if (chunk_len > 0) {
        chunk_count++;
        mvar local_var;
        cstring var_value;
        memset(&local_var, 0, sizeof(mvar));
        local_var.uci = UCI_IS_LOCALVAR;
        local_var.slen = 0;
        snprintf((char *)local_var.name.var_cu, sizeof(local_var.name.var_cu), "%c%d", res_prefix[0], chunk_count);
        memcpy(var_value.buf, chunk_buffer, chunk_len);
        var_value.len = chunk_len;
        ST_Set(&local_var, &var_value);
    }
    free(chunk_buffer);
    sqlite3_finalize(stmt);
    set_mumps_row_count(total_rows);
    return chunk_count;
}
int execute_sqlite_non_query(const char *sql_text) {
    sqlite3 *db = get_sqlite_connection();
    if (!db) return -1;
    char *err_msg = NULL;
    int rc = sqlite3_exec(db, sql_text, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        char err_buf[MAX_STR_LEN];
        snprintf(err_buf, sizeof(err_buf), "[SQLite Exec Error] %s", err_msg ? err_msg : sqlite3_errmsg(db));
        if (err_msg) sqlite3_free(err_msg);
        set_mumps_sql_error(err_buf);
        return -1;
    }
    const char *ptr = sql_text;
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r') {
        ptr++;
    }
    if (strncasecmp(ptr, "INSERT", 6) == 0 || 
        strncasecmp(ptr, "UPDATE", 6) == 0 || 
        strncasecmp(ptr, "DELETE", 6) == 0) {
        return sqlite3_changes(db);
    }
    return 1;
}