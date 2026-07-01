# yChat — Mode B: embedded SQLite (real accounts, no MySQL server)

Mode A (`Dockerfile`, `DOCKER.md`) is an in-memory guest chat with no account
database. This is **Mode B**: `DATABASE` is enabled and backed by an embedded
SQLite file instead of MySQL, so registration/login persist across restarts
without depending on an external database server.

This is a local proof-of-concept build, **not deployed to f3s** — the live
cluster still runs Mode A (`https://ychat.f3s.lan.buetow.org/`).

## Build & run (local)

```sh
cd ychat
podman build -t ychat:sqlite -f Dockerfile.sqlite .
mkdir -p /path/to/data && chmod 777 /path/to/data   # see note below
podman run --rm -p 2000:2000 -v /path/to/data:/app/data:Z ychat:sqlite
```

Open http://localhost:2000/ — this is the **real** login page (password
field, "Register" link), since `DATABASE` is enabled. Register a nick,
restart the container, log back in with the same password: it works, because
the database is the file at `/app/data/ychat.db` (bind-mounted).

**Rootless-podman note:** a bind-mounted host directory is usually not
writable by the container's non-root `ychat` user (UID 1000) because of user
namespace remapping. `chmod 777` on the host directory is the quick fix for
local testing; for a real deployment use a named volume or a properly
`chown`ed hostPath/PVC instead.

## What changed vs. Mode A

- `src/configure.ac` / `src/configure`: added `--enable-sqlite` (checks for
  `sqlite3.h` / `-lsqlite3`), mirroring the existing `--enable-mysql`
  machinery. `src/config.h.in`/`src/glob.h`: when both are detected,
  `HAVE_SQLITE3_H`+`HAVE_LIBSQLITE3` define `USE_SQLITE` + `DATABASE`
  (mirrors the existing `HAVE_MYSQL_MYSQL_H`+`HAVE_LIBMYSQLCLIENT` ->
  `USE_MYSQL`+`DATABASE` block).
- `src/data/con.h`/`con.cpp`: `#ifdef USE_SQLITE` branch opens a
  `sqlite3*` instead of `MYSQL*`, sets `PRAGMA journal_mode=WAL` +
  a busy timeout (multiple pooled connections open the same file
  concurrently; this data layer has no query-retry logic of its own, so a
  writer needs to wait for a lock rather than fail immediately with
  `SQLITE_BUSY`), and runs `CREATE TABLE IF NOT EXISTS user (...)` -
  MySQL deployments are expected to have this table created out-of-band;
  SQLite has no such step, so bootstrap it on first connect.
- `src/data/data.h`/`data.cpp`: `#ifdef USE_SQLITE` branch rebuilds
  `select_user_data`/`insert_user_data`/`update_user_data` on
  `sqlite3_prepare_v2`/`sqlite3_bind_text`/`sqlite3_step` (parameterized
  queries) instead of hand-built SQL strings. This is safer than the MySQL
  path's `secure_query()`, which prevents injection by *transliterating*
  `"`/`\` to `'`/`/` rather than escaping them (crude but functional for
  MySQL; parameter binding sidesteps the whole class of problem for SQLite,
  so there's no SQLite equivalent of `secure_query()`).
- `etc/ychat.conf`: `chat.database.dbname`'s description now notes it
  doubles as the SQLite file path in this mode (`serverhost`/`user`/
  `password`/`port` are unused).
- **Renamed `class data` to `class ychatdb`** (`data.h`/`data.cpp`,
  `wrap.h`/`wrap.cpp`, `mods/html/yc_register.cpp`): a class literally named
  `data` collides with `std::data()` (C++17) under `using namespace std` -
  GCC 11 reports "reference to 'data' is ambiguous". This is the *third*
  instance of this exact bug class found across this revival (see the
  `function`->`mod_func_t` rename in `glob.h`/`modl.cpp`, and
  `attributes::set`->`set_attr_flag` in `../ycurses`) - all three are
  1990s/2000s-era C++ that picked short, common names later claimed by the
  standard library, invisible until `using namespace std` + a modern
  standard collide them.
- **Fixed an infinite-loop OOM in `data_base.cpp`'s query-config parser**:
  `unsigned i_pos` truncating `string::npos` (the *exact* same bug class as
  the `unsigned`-vs-`size_t` fixes already made across `../yhttpd`) made the
  last-token check `i_pos != string::npos` always true, and `i_pos+1`
  wrapped back to `0` in 32-bit arithmetic - so the loop never advanced or
  terminated, growing a `vector<string>` forever. Fixed to `size_t`. This
  had never been hit before: `DATABASE` was never actually compiled
  previously (Mode A disables it, and `--enable-mysql` was separately
  broken - see below), so this whole code path was completely untested
  until Mode B exercised it for the first time.

## Verified

Built and run in a Rocky Linux 9 container (matching Mode A's toolchain):
register (`POST register.html`) creates a row in the SQLite `user` table;
login (`POST frameset.html`) with the correct password succeeds (returns the
chat frameset) and with a wrong password is rejected
(`chat.msgs.err.wrongpassword`); a second registered user's login still
works identically after a full `podman restart` (proving the SQLite file
persisted the account, not just an in-memory cache); 15 sequential requests
post-restart all `200`, no crashes/restarts. `update_user_data`
(`savechangednick`, used when a logged-in user changes options) uses the
same prepare/bind/step pattern as the verified insert/select paths but
wasn't independently exercised over HTTP (it needs an authenticated session
cookie) - verified by code inspection only.

## Known, pre-existing, deliberately NOT fixed

- **`--enable-mysql` doesn't work**, independent of anything here:
  `configure.ac` registers the option as `AC_ARG_ENABLE(mysqlclient, ...)`
  (setting `$enable_mysqlclient`) but the help text advertises
  `--enable-mysql`, and the actual gating check later tests `$enable_mysql`
  - a third, never-set variable. So MySQL support has likely never been
  selectable via `./configure` since this script was written. Left alone:
  this task is about moving *away* from MySQL, not fixing it.
- **Passwords are stored and compared in plaintext** (`yc_register.cpp`,
  `chat.cpp`'s login check) - this predates the SQLite work (same behavior
  as the MySQL path) and is a bigger, separate concern than "swap the
  database backend"; not addressed here.
- **`data::secure_query()`'s MySQL-only escaping** (`data.cpp`, `#else`
  branch) has the same `unsigned i_pos != string::npos` bug as the one
  fixed in `data_base.cpp` above. Unreached by this build (`USE_SQLITE` is
  defined, so the `#else` branch never compiles here) and MySQL is
  unreachable anyway per the point above - not fixed, since fixing dead
  code invites bit-rot without a way to verify it.
