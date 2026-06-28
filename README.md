# Hybrid MUMPS-SQL Enterprise Engine 🚀
### Architectural White Paper & Vision

A lightweight, ultra-fast, modern SQL Gateway designed specifically for the **Raymond Douglas Newman MUMPS Engine**. This project introduces a production-ready paradigm shift: executing relational SQL operations directly inside a classic hierarchical tree database (MUMPS), without altering the compiler core.

---

## 1. Executive Summary & The Challenge
Traditional MUMPS architectures excel at handling massive hierarchical structures (Globals) via high-speed, local B-trees. However, integrating modern relational analytics (SQL) usually requires heavy ETL pipelines or modifying deep compiler logic.

**Our Solution:** A "Top-Down" hybrid design that hijacks standard M string-manipulation syntax (`$PIECE`) to pass execution payloads directly to a dynamic, native-C multi-database coordinator. 

### 💡 The Dual-Storage Paradigm (Glow + SQL)
It is crucial to understand that **this system does not replace MUMPS.** The architecture runs in a pure **Tidal Duality**: Your existing MUMPS Routines and Global storage (Glow) continue to operate natively with zero overhead. Relational SQLite tables run in parallel on the same core, managed by the C Gateway, giving developers the "best of both worlds" directly from the native `M>` prompt.

---

## 2. Architectural Overview (The "Trampoline" Mechanism)
Instead of redesigning the parser, we intercept runtime data evaluation through a clean, non-intrusive hook. 

* **Step 1:** MUMPS Code calls the API -> `$$QUERY^SQLAPI(DB, SQL, Prefix)`
* **Step 2:** Interception occurs via the custom `$PIECE` Hook.
* **Step 3:** The Native C Runtime (`db_manager.c`) executes the query on the Local SQLite Database.
* **Step 4:** **Dynamic High-Velocity Variable Injection:** The C layer automatically streams results back into the MUMPS Symbol Table. Rows are packed into 32KB packed memory chunks (e.g., `R1`, `R2...`) separated by binary delimiters, completely eliminating Symbol Table saturation and Junk Pointer errors during massive datasets.

---

## 3. Core Technical Implementations
* **Zero-Compiler Modification:** Leverages native runtime execution pathways.
* **ACID Transactions & Persistent Sessions:** Each unique MUMPS Job/Session initializes a dedicated, persistent static connection pool to the SQLite databases on the storage layer. This architecture explicitly enables **ACID Compliant Transactions** directly from MUMPS routines, allowing developers to manage `BEGIN TRANSACTION`, `COMMIT`, and `ROLLBACK` lifecycles on relational data.
* **Ultra-Fast Chunking:** Row results are packed and bound directly to fast-access symbols (e.g., `R1`, `R2`) delimited via native binary characters, keeping runtime computational overhead close to zero.

---

## 4. High-Level MUMPS API Wrapper
MUMPS developers don't have to worry about the internal C mechanics or pipeline tricks. The engine exposes a clean, high-level interface:

* `$$DDL^SQLAPI(DB, Command)` - Schema generation and database initialization.
* `$$DML^SQLAPI(DB, Command)` - Transactional Insert, Update, and Delete operations.
* `$$QUERY^SQLAPI(DB, Command, Prefix)` - Executes relational queries and binds packed fast variables dynamically.

---

## 5. Current Project Status & Roadmap
* **Phase 1:** Core C Interceptor Concept & Testing.
* **Phase 2:** Native SQLite integration with Acid Transaction support.
* **Phase 3:** Production-Grade High-Velocity Chunking architecture (R1-R{N}).
* **Phase 4:** Demo suite, Installation Guidelines, and Developer Documentation.
* **Phase 5:** Cloud Remote PostgreSQL connectivity integration (Hybrid Layer).

The architecture is natively decoupled, allowing the Multi-Database Coordinator to route commands dynamically based on the payload, paving the way for future enterprise database connectors such as PostgreSQL, MySQL, or Oracle without altering the core runtime engine.

---

## Documentation & Guides
Custom high-performance relational gateway developer manuals:

* 📘 [Programmer's Integration Guide](https://www.test-prep.oo.gd/Testprep//programmer_guide.html)
* 💻 [M2SQL Shell REPL User Guide](https://www.test-prep.oo.gd/Testprep/m2sql_guide.html)

*

---
*Developed by Rafi Farchi. Inspired by the work of Raymond Douglas Newman and Sam Habiel.*
