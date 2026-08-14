# ychat / yhttpd / ycurses

This repository collects three small, legacy C++ projects by Paul C. Buetow
(originally ~2003–2007). They share a common socket/event/template engine and
are kept here as historical/revival code.

| Subproject | What it is | Status |
|------------|------------|--------|
| [`./ychat`](ychat/)   | An HTTP-based web chat server (browsers are the clients; CSS/HTML/JS only). | **Revived, builds in Docker with a mandatory embedded-SQLite backend** (real, persistent registered accounts) — see [`ychat/DOCKER.md`](ychat/DOCKER.md). |
| [`./yhttpd`](yhttpd/) | A tiny standalone http server derived from ychat's socket/threading engine. | Builds and serves reliably in Docker (verified under concurrent load) — not deployed. See [`./yhttpd/DOCKER.md`](yhttpd/DOCKER.md). |
| [`./ycurses`](ycurses/) | A curses front-end experiment. | Builds and runs in Docker (a demo, not a service, so nothing to deploy) — see [`./ycurses/BUILD.md`](ycurses/BUILD.md). |

The detailed, up-to-date build/deploy notes for the chat live in
[`./ychat/DOCKER.md`](ychat/DOCKER.md). The rest of this file is a
quickstart for running **ychat** locally in Docker and accessing it.

> The ychat tree has been substantially fixed during this revival (legacy-C++
> build fixes, a from-scratch streaming-chat layer, and a security/bug sweep).
> See `git log` under `./ychat` and `./ychat/DOCKER.md` for the full list.

---

## Quickstart: run ychat locally in Docker

You need a container runtime (`podman` or `docker`). The build is a
multi-stage `Dockerfile` (Rocky Linux 9 builder + slim Rocky 9 runtime) that
compiles ychat **entirely inside the container** — no host toolchain required.

### 1. Build the image

From the **repository root**:

```sh
cd ychat
podman build -t ychat:dev .
# or:  docker build -t ychat:dev .
```

The build configures ychat with SSL and readline off, but a database is not
optional: `./configure` always requires SQLite (`sqlite3.h`/`libsqlite3`),
so registration/login persist in a SQLite file across container restarts.
The default chat port is **2000**.

### 2. Run it

```sh
mkdir -p /tmp/ychat-data && chmod 777 /tmp/ychat-data   # see DOCKER.md for why
podman run --rm -p 2000:2000 --name ychat -v /tmp/ychat-data:/app/data:Z ychat:dev
# or:  docker run --rm -p 2000:2000 --name ychat -v /tmp/ychat-data:/app/data ychat:dev
```

The server logs to stdout. You should see something like:

```
yChat 0.9.5-CURRENT Build ...
Sock: Created socket on localhost:2000
Sock: Server socket is ready
Initializing sock events (1)
```

### 3. Access it

Open http://localhost:2000/ in a browser.

- You'll get the full login page (password field + "Register" link). You can
  register a nick/password (persisted in the SQLite file under
  `/app/data`), or leave the password blank and log in as an unregistered
  guest — `chat.enableguest=true` allows that regardless of the database.
- Enter any alphanumeric nick (e.g. `alice`), leave the room as `Lounge`, and
  click **login**.
- The chat frameset loads: a streaming message view, the online-user list, and
  an input box. Type a message and hit **Send** — it appears in the stream.
- Open a second browser/window with a different nick in the same room to see
  messages delivered to both clients in real time.

Quick CLI checks:

```sh
# the login page (HTTP/1.1, 200):
curl -sS http://localhost:2000/ -o /dev/null -w '%{http_code}\n'

# log in and grab a session tmpid:
curl -sS -X POST -d 'event=login&nick=alice&room=Lounge&end=end' \
     http://localhost:2000/frameset.html
```

### 4. Stop it

```sh
podman rm -f ychat
# (Ctrl-C also stops the foreground `run` above)
```

---

## Notes on the local run

- **Registered accounts persist; guest sessions don't.** The SQLite file at
  `/app/data/ychat.db` (bind-mount it, as above, to survive container
  restarts) holds registered users. Sessions/rooms/online-state are still
  in-memory, and unregistered `chat.enableguest=true` guest chatters are
  wiped on restart same as before — only the accounts table persists.
- **Logs** go to `/app/log/` inside the container (`access_log`, `system_log`,
  `rooms/<room>`). They're a container-local dir locally, so they don't
  persist after `rm`.
- **Configuration** is `ychat/etc/ychat.conf`, baked into the image at
  `/app/etc/ychat.conf`. You can override any config key at runtime with
  `-o <key> <value>` (the image already does this for
  `chat.session.md5hash=false` and `chat.database.dbname=data/ychat.db`).
  Example: `podman run --rm -p 2000:2000 ychat:dev /app/bin/ychat -o chat.idle.timeout 300`.
- **The `/exec` command module is removed from the image entirely**
  (defense-in-depth against its shell-injection RCE), and operator status
  via `chat.defaultop` now requires a database-authenticated registered
  account — an unregistered guest can never claim it. Other privileged
  commands (`/ko`, `/ban`, …) work normally for a registered operator.

---

## License

GPL-2.0 (see [`./ychat/COPYING`](ychat/COPYING)). Source:
https://codeberg.org/snonux/ychat

ychat's web UI vendors [Pico CSS](https://picocss.com)
(`ychat/html/pico.min.css`), which is MIT-licensed; its copyright/license
notice is kept in the file's header comment. Pico is a separate,
MIT-licensed dependency and is not covered by ychat's own GPL-2.0 license.