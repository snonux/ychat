# yhttpd — Docker revival (in progress)

`yhttpd` is a minimal standalone http server derived from ychat's
socket/threading engine (originally ~2003–2005). It is a *subset* of ychat —
the `yhttpd` tree was generated from the ychat source via
`scripts/makeyhttpd.pl`.

This revival ports the relevant fixes from `../ychat` and gets yhttpd building
on a modern toolchain. **It builds in Docker (Rocky Linux 9 / GCC 11) and
serves sequential and concurrent requests reliably** (verified: 100/100
requests across 5 bursts of 20 concurrent, 0 TCP resets) — see *Status*
below for the one remaining known issue on newer host toolchains.

## Build & run (local)

```
podman build -t yhttpd:dev .
podman run --rm -p 2000:2000 yhttpd:dev
curl http://localhost:2000/      # first request returns 200
```

The server listens on port **2000** (`httpd.serverport` in `etc/yhttpd.conf`).
Static pages live in `html/`; logs in `/app/log/`.

## What was fixed (ported from ychat + yhttpd-specific)

yhttpd shares ychat's buggy files, so most ychat fixes apply. Ported / fixed:

- **Build (modern g++):** `src/glob.h` renamed the project's `typedef int
  function(...)` to `mod_func_t` (it collided with `std::function` under
  `using namespace std;`); `configure`'s g++ 3.x version gate now accepts any
  GNU g++; `config.pl` is fed `yes` (defaults) non-interactively; `Makefile`
  perl invocations use `-I.` (modern Perl dropped `.` from `@INC`).
- **`logd::flush`:** `ofstream == NULL` → `!is_open()`, and a missing logfile
  is reported to stderr + `exit(1)` instead of recursing through
  `system_message` → `log_simple_line` → `flush` (stack overflow).
- **`tool::trim`:** rewrote the right-trim (the old `s_str[s_str.size()]` is
  out-of-bounds under `_GLIBCXX_ASSERTIONS` / UB).
- **`tool::shell_command` (CGI):** replaced `popen("/bin/sh -c …")` with
  `fork/execve` of the file directly (no shell), so URL-derived metacharacters
  can't inject when `httpd.enablecgi=true` (dormant/off by default).
- **`tool::replace` and the pervasive `unsigned` string positions:** yhttpd
  declared many find/substr/replace positions as `unsigned` (32-bit) but
  `std::string::find` returns `size_t` (64-bit) `npos`. Truncating `npos` to
  `unsigned` makes `pos == string::npos` *false*, so the wrong branch runs /
  `substr`/`replace` get `pos=0xFFFFFFFF` → `out_of_range`/infinite recursion.
  Fixed all such sites (`reqp::get_url`, `reqp::get_request_parameters`,
  `reqp::parse_headers`' helper, `reqp::remove_dots`, `html::parse`,
  `conf::get_vector`, `logd::set_logfile`, `logd::remove_html_tags`,
  `tool::split_string`, `tool::replace`) to `size_t`. (ychat used `int`, where
  `int(npos)==-1` compares equal to `npos`, so it didn't hit this — yhttpd's
  `unsigned` did.)
- **`accept`/`getpeername` `size_t`→`socklen_t`** (modern glibc is strict).
- **`www.yChat.org` links** → `https://codeberg.org/snonux/ychat`.
- **NCURSES/CLI disabled** in `glob.h` (no ncurses dep; pure httpd), which
  needed a `ncur => ["ncur"]` entry added to `src/configure`'s
  `%dependfiles` (mirroring the existing `cli`/`database`/`ycurses`
  move-aside-when-disabled handling) so disabling `NCURSES` actually moves
  `src/ncur/` out of the build tree, matching how the Dockerfile doesn't
  install `ncurses-devel`.

**Corrected claim (originally in this file):** an earlier pass here claimed
`sock::_make_server_socket` was "never called" and added a call to it in
`sock::start()`. That was a misdiagnosis, caught by review: `wrap::
init_wrapper()` (`wrap.cpp`) already calls it, before `main.cpp` ever calls
`SOCK->start()`. The added call in `start()` ran a **second**
`_make_server_socket`, which happened to work (Linux allows a second
`bind()` to the same wildcard address before either socket calls `listen()`)
but leaked the first fd and, had `OPENSSL` ever been enabled, would have
run `sslsock`'s override a second time (leaking the first `SSL_CTX`,
re-reading the cert/key files). `sock::start()` now just uses the
already-initialized `i_server_sock` instead of calling
`_make_server_socket()` again.

## Status — stable, including under concurrent load

With the above, yhttpd **builds on GCC 11** (it didn't before). Note this is
built and run in Docker (Rocky Linux 9 / GCC 11 / older glibc) — building
directly on the host (Fedora, GCC 16, newer glibc heap checks) reliably
crashed with `SIGILL` in `sock::_close` (`delete p_sock`) after the first
request. That crash does **not** reproduce in the container: 100/100
requests across 5 bursts of 20 concurrent all returned `200`, and the
process never restarted or exited. This suggests the corruption is real but
latent — the older toolchain/glibc in the container just doesn't trip over
it — rather than fixed. Treat `SIGILL` under `sock::_close` as a known
landmine if this is ever rebuilt on a newer host toolchain.

Two concurrency bugs were found and fixed while investigating intermittent
`curl` `000`/"connection reset by peer" failures under concurrent load:

- **`listen()` backlog was hardcoded to `1`.** Bumped to `SOMAXCONN`
  (`sock.cpp`, `start()`), since the `select()`-based accept loop only calls
  `accept()` once per event-loop iteration and needs the kernel to hold a
  real backlog of pending connections in the meantime.
- **The real cause of the resets: `sock::_close()` closed sockets with
  unread data still in the kernel receive buffer.** `read_http()` only ever
  consumes the GET request line and intentionally ignores the rest of the
  headers the client already sent. Closing a socket like that makes Linux
  send an abortive RST instead of a graceful FIN — which can race the
  client's read of the response just sent and surface as "connection reset
  by peer" even though the response was fully delivered. Fixed by
  half-closing the write side, then a **non-blocking** drain of whatever's
  already in the receive buffer before the real `close()` — the client
  already sent its whole request in one burst before we got here (we'd
  already read the first line via blocking reads), so the remaining bytes
  are already buffered and a non-blocking read picks them up with no added
  latency (a timed/blocking drain was tried first but rejected: ordinary
  HTTP clients wait on the response instead of half-closing first, so it
  would pay its full timeout on nearly every request). Confirmed via
  `tcpdump`: before the fix, RSTs followed every response; after, zero RSTs
  across 140 requests over multiple concurrent bursts.

Not investigated: the thread pool (`src/thrd/pool.cpp`)'s worker wakeup uses
`pthread_cond_signal` from `add_task()` without holding the same mutex the
waiting threads block on (`mut_threads`) — a classic lost-wakeup pattern
under POSIX. It didn't surface in this testing (concurrency is currently
bounded by the fix above), but is worth a closer look before relying on
yhttpd under sustained heavy concurrency.

## Files

- `src/` — sources (NCURSES/CLI sources are still present in the repo; the
  build's `src/configure` moves `ncur/` + `cli/` aside when those are
  disabled, at build time).
- `etc/yhttpd.conf` — config (port, templatedir, logging).
- `html/` — served static pages.