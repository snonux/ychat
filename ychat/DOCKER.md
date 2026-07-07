# yChat — Docker revival (embedded SQLite backend)

yChat is a legacy (2007) C++ HTTP chat server. This revival builds it
**entirely in a Docker container**. A database is no longer optional: the
build always compiles in `DATABASE`, backed by an embedded SQLite file, so
registration and login persist across container restarts. There is no more
"no-DB" build option — see "History" below if you're wondering why some
older commits/docs mention one.

## Build

Multi-stage `Dockerfile`: Rocky Linux 9 builder (GCC 11 tolerates the legacy
C++ this tree uses) + slim Rocky 9 runtime.

```sh
cd ychat
podman build -t ychat:dev .
# or:  docker build -t ychat:dev .
```

## Run

```sh
mkdir -p /path/to/data && chmod 777 /path/to/data   # see rootless-podman note below
podman run --rm -p 2000:2000 -v /path/to/data:/app/data:Z ychat:dev
# smoke test (ychat returns proper HTTP/1.1 responses):
curl -sS http://127.0.0.1:2000/index.html -o /dev/null -w '%{http_code}\n'
```

Open http://localhost:2000/ — this is the full login page (password field,
"Register" link). Register a nick, restart the container, log back in with
the same password: it works, because the account lives in the SQLite file at
`/app/data/ychat.db` (bind-mounted, so it survives the container being
recreated).

**Rootless-podman note:** a bind-mounted host directory is usually not
writable by the container's non-root `ychat` user (UID 1000) because of user
namespace remapping. `chmod 777` on the host directory is the quick fix for
local testing; for a real deployment use a named volume or a properly
`chown`ed hostPath/PVC instead.

## Legacy-C++ patches applied

The tree does not build on a modern toolchain unmodified. A handful of
minimal, semantics-preserving patches were made so it builds on Rocky 9 /
GCC 11 (see `git log` for the full, ongoing list; highlights):

- `src/glob.h`, `src/modl.cpp`: renamed the project's `typedef int function(...)`
  type to `mod_func_t` — it collided with `std::function` brought in by
  `using namespace std;` under C++11.
- `src/logd.cpp`: `ofstream == NULL` → `!ofstream.is_open()` (streams are no
  longer comparable to `NULL`).
- `src/sock/sock.cpp`: reordered `i_server_sock = i_sock;` to *before* the
  `set_nonblock(i_server_sock)` call — previously `set_nonblock` ran on an
  uninitialised member, returning EBADF and aborting startup.
- `src/data/data.h`/`data.cpp`, `src/wrap.h`/`wrap.cpp`,
  `src/mods/html/yc_register.cpp`: renamed `class data` to `class ychatdb` —
  a class literally named `data` collides with `std::data()` (C++17) under
  `using namespace std` ("reference to 'data' is ambiguous"). Same bug class
  as the `function`→`mod_func_t` rename above.
- `src/data/data_base.cpp`: `unsigned i_pos` → `size_t i_pos` in the
  config-query token parser — truncating `string::npos` to 32-bit made the
  "no more tokens" check never true and `i_pos+1` wrap back to `0`, so the
  loop never terminated (an infinite loop that OOM-killed the container
  within seconds of startup, the first time `DATABASE` was ever actually
  compiled and run).

## Runtime layout (WORKDIR /app)

| Path | Purpose | Writable? |
|------|---------|-----------|
| `bin/ychat` | server binary | no |
| `etc/ychat.conf` | config (found via `./etc/` search path) | no |
| `html/` | templates, including `register.html`/`options.html` | no |
| `mods/{commands,html}/*.so` | runtime-loadable modules | no |
| `log/` | access/system/room logs | yes (mount a volume) |
| `data/` | SQLite db file (`ychat.db`) | yes (mount a volume for persistence) |

## What the database backend looks like

- `src/configure.ac`/`src/configure`: `sqlite3.h`/`-lsqlite3` are checked
  unconditionally (like `pthread`/`libevent`) — there's no `--enable-sqlite`
  flag any more, it isn't optional. `--enable-mysql` still exists
  (`glob.h`'s `USE_MYSQL`/`USE_SQLITE`/`DATABASE` mechanism supports either
  backend) but is a pre-existing, separately-broken code path
  (`configure.ac` registers it as `AC_ARG_ENABLE(mysqlclient,...)` but the
  gating check tests a different, never-set `$enable_mysql`) that this
  revival deliberately leaves alone — the direction is away from MySQL, not
  toward fixing it.
- `src/data/con.h`/`con.cpp`: the `USE_SQLITE` branch opens a `sqlite3*`,
  sets `PRAGMA journal_mode=WAL` plus a busy timeout (multiple pooled
  connections open the same file concurrently; this data layer has no
  query-retry logic of its own, so a writer needs to wait for a lock rather
  than fail immediately with `SQLITE_BUSY`), and runs
  `CREATE TABLE IF NOT EXISTS user (...)` on first connect (SQLite has no
  separate schema-provisioning step, unlike a MySQL deployment where the
  table is expected to exist already).
- `src/data/data.h`/`data.cpp`: `select_user_data`/`insert_user_data`/
  `update_user_data` use `sqlite3_prepare_v2`/`sqlite3_bind_text`/
  `sqlite3_step` (parameterized queries), which is safer than the MySQL
  path's `secure_query()` — that escapes by *transliterating* `"`/`\` to
  `'`/`/` rather than properly escaping them.
- `etc/ychat.conf`: `chat.database.dbname` doubles as the SQLite file path
  (`serverhost`/`user`/`password`/`port` only matter for a `--enable-mysql`
  build).
- Unregistered **guest logins are still a supported runtime feature**,
  independent of the database: `chat.enableguest=true` in `ychat.conf` lets
  anyone log in with just a nick and no password (see `src/chat/chat.cpp`).
  A guest's `is_reg` is always `false`, so a guest can never be granted
  operator status via `chat.defaultop` — only a database-authenticated
  registered account can.

> Note: ychat emits proper HTTP/1.1 responses (`HTTP/1.1 200 OK` + headers).
> Earlier in the revival it sent headerless HTTP/0.9-style bodies (the
> response builder left the headers in a local string and never wrote them
> back to the socket buffer); that was fixed in `src/reqp.cpp`.

## Verified

Built and run in a Rocky Linux 9 container: register (`POST register.html`)
creates a row in the SQLite `user` table; login (`POST frameset.html`) with
the correct password succeeds (returns the chat frameset) and with a wrong
password is rejected (`chat.msgs.err.wrongpassword`); a registered user's
login still works identically after a full container restart/recreate
(proving the SQLite file persisted the account, not just an in-memory
cache); the input frame shows Colors/Options/Help/Users/Admin/Logout; both
`options.html` and `register.html` resolve (not "Page not found").

## Known, pre-existing, deliberately NOT fixed

- **`--enable-mysql` doesn't work** (see above) — left alone; this revival
  moves away from MySQL rather than fixing it.
- **Passwords are stored and compared in plaintext**
  (`src/mods/html/yc_register.cpp`, `src/chat/chat.cpp`'s login check) — a
  bigger, separate concern than "always require a database backend"; not
  addressed here.
- **`data::secure_query()`'s MySQL-only escaping** (`data.cpp`, the
  `USE_MYSQL` branch) has the same `unsigned i_pos != string::npos` bug
  class fixed in `data_base.cpp` above — unreached by this build
  (`USE_SQLITE` is what actually gets compiled) and MySQL is unreachable
  anyway per the point above, so not fixed: fixing dead code invites
  bit-rot without a way to verify it.

## History

Earlier in this revival there were briefly two build modes: an in-memory,
no-database "Mode A" guest chat (the original, minimal revival) and an
opt-in `--enable-sqlite` "Mode B" alongside it (`Dockerfile.sqlite`,
`DOCKER-SQLITE.md`). A later task removed Mode A entirely: `DATABASE` (and
therefore SQLite) is now mandatory, there's only one `Dockerfile`, and the
build-aware UI toggles that existed to make Mode A's lack of a database less
confusing (guest-only login page, hidden Register link, hidden Options menu)
were reverted, since they no longer apply — the full login/registration/
options UI is always functional now. Unregistered guest chatting itself was
**not** removed — `chat.enableguest` is a runtime config option, not a
compile flag, and stayed as-is.

## Optional follow-ups (not in scope for this task)

- Port to a modern toolchain (`hash_map`→`unordered_map`, fix implicit
  returns, modernise `configure.ac`) and drop the legacy-C++ patches.
- Fix plaintext password storage/comparison.
