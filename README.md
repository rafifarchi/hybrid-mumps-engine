2# Hybrid MUMPS-SQL Enterprise Engine 🚀
### Architectural White Paper & Vision

A lightweight, ultra-fast, modern SQL Gateway designed specifically for the **Raymond Douglas Newman MUMPS Engine**. This project introduces a production-ready paradigm shift: executing relational SQL operations directly inside a classic hierarchical tree database (MUMPS), without altering the compiler core.

---

## 1. Executive Summary & The Challenge
Traditional MUMPS architectures excel at handling massive hierarchical structures (Globals) via high-speed, local B-trees. However, integrating modern relational analytics (SQL) usually requires heavy ETL pipelines or modifying deep compiler logic.

**Our Solution:** A "Top-Down" hybrid design that hijacks standard M string-manipulation syntax (`$PIECE`) to pass execution payloads directly to a dynamic, native-C multi-database coordinator. 

---

## 2. Architectural Overview (The "Trampoline" Mechanism)
Instead of redesigning the parser, we intercept runtime data evaluation through a clean, non-intrusive hook. 

* **Step 1:** MUMPS Code calls the API -> `$$QUERY^SQLAPI(DB, SQL, VAR)`
* **Step 2:** A standard JSON payload is built dynamically.
* **Step 3:** Interception occurs via the custom `$PIECE` Hook.
* **Step 4:** The Native C Runtime (`db_manager.c`) executes the query on the Local SQLite Database.
* **Step 5:** Fast Variable Injection automatically streams results straight back into the MUMPS Symbol Table as flat local variables (e.g., `USR1`, `USR2`, `USR3...`) for maximum performance.

---

## 3. Core Technical Implementations
* **Zero-Compiler Modification:** Leverages native runtime execution pathways.
* **Persistent Connection Pooling:** Maintains static, lifecycle-managed connections to SQLite databases on the storage layer.
* **Flat-Variable Injection:** Row results are fetched and bound directly to fast-access symbols (e.g., `USR1`, `USR2`) delimited via native pipe character (`|`), keeping runtime computational overhead close to zero.

---

## 4. High-Level MUMPS API Wrapper
MUMPS developers don't have to worry about JSON formatting or pipeline tricks. The engine exposes a clean, high-level interface:

* `$$DDL^SQLAPI(DB, Command)` - Schema generation and database initialization.
* `$$DML^SQLAPI(DB, Command)` - Insert, Update, and Delete operations.
* `$$QUERY^SQLAPI(DB, Command, TargetVar)` - Executes relational queries and binds fast flat variables dynamically.

---

## 5. Current Project Status & Roadmap
* **Phase 1:** Core C Interceptor Concept & Testing.
* **Phase 2:** Native SQLite integration (File-Persistent & In-Memory storage).
* **Phase 3:** Developer Documentation, Demo suite, and Installation Guidelines (Current).
* **Phase 4:** Cloud Remote PostgreSQL connectivity integration (Hybrid Layer).

The architecture is natively decoupled, allowing the Multi-Database Coordinator to route commands dynamically based on the JSON payload, paving the way for future enterprise database connectors such as PostgreSQL, MySQL, or Oracle without altering the core runtime engine.

## Documentation & Guides
Custom high-performance relational gateway developer manuals:

* 📘 [Programmer's Integration Guide](https://www.your-site.com/programmer_guide.html)
* 💻 [M2SQL Shell REPL User Guide](https://www.your-site.com/m2sql_shell_guide.html)



*

---
*Developed by Rafi Farchi. Inspired by the work of Raymond Douglas Newman and Sam Habiel.*
