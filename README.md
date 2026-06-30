# ychat / yhttpd / ycurses

This repository collects three small, legacy C++ projects by Paul C. Buetow
(originally ~2003–2007). They share a common socket/event/template engine and
are kept here as historical/revival code.

| Subproject | What it is | Status |
|------------|------------|--------|
| [`./ychat`](ychat/)   | An HTTP-based web chat server (browsers are the clients; CSS/HTML/JS only). | **Revived & deployed** — builds in Docker, runs on the f3s k3s cluster. |
| [`./yhttpd`](yhttpd/) | A tiny standalone http server derived from ychat's socket/threading engine. | Unrevived (see its own tree). |
| [`./ycurses`](ycurses/) | A curses front-end experiment. | Unrevived (see its own tree). |

The detailed, up-to-date build/deploy notes for the chat live in
[`./ychat/DOCKER.md`](ychat/DOCKER.md). The rest of this file is a quickstart
for running **ychat** locally in Docker and accessing it.

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

The build configures ychat with all optional features off (no SSL, no MySQL,
no readline) — this is "Mode A": an **in-memory guest chat with no account
database**. The default chat port is **2000**.

### 2. Run it

```sh
podman run --rm -p 2000:2000 --name ychat ychat:dev
# or:  docker run --rm -p 2000:2000 --name ychat ychat:dev
```

The server logs to stdout. You should see something like:

```
yChat 0.9.0-CURRENT Build ...
Sock: Created socket on localhost:2000
Sock: Server socket is ready
Initializing sock events (1)
```

### 3. Access it

Open http://localhost:2000/ in a browser.

- You'll get the **guest login page** (no password field, no "Register" link —
  there is no account database in this build).
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

- **State is in-memory only.** With no database, all users/sessions/rooms live
  in RAM and are wiped on container restart. That's intentional for the
  revival; `chat.enableguest=true` lets anyone log in with just a nick.
- **Logs** go to `/app/log/` inside the container (`access_log`, `system_log`,
  `rooms/<room>`). They're an `emptyDir` in k8s and a container-local dir
  locally, so they don't persist after `rm`.
- **Configuration** is `ychat/etc/ychat.conf`, baked into the image at
  `/app/etc/ychat.conf`. You can override any config key at runtime with
  `-o <key> <value>` (the image already does this for
  `chat.session.md5hash=false` and `httpd.startsite=index_guest.html`).
  Example: `podman run --rm -p 2000:2000 ychat:dev /app/bin/ychat -o chat.idle.timeout 300`.
- **No operator commands for guests.** The default-operator escalation
  (`/exec` shell RCE) was removed for security; in this no-DB build there is no
  authenticated operator, so privileged commands (`/ko`, `/ban`, `/exec`, …)
  are unavailable by design.

---

## Deploying to the f3s k3s cluster

This is covered in detail in [`./ychat/DOCKER.md`](ychat/DOCKER.md). In short:
the image is pushed to the f3s private registry
(`r0.lan.buetow.org:30001/ychat:<tag>`), and a Helm chart + ArgoCD Application
in the [`conf` repo](https://codeberg.org/snonux/conf) (path
`f3s/ychat/helm-chart`) deploy it. The LAN URL is
**https://ychat.f3s.lan.buetow.org/**.

---

## License

GPL-2.0 (see [`./ychat/COPYING`](ychat/COPYING)). Source:
https://codeberg.org/snonux/ychat