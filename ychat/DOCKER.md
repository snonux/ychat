# yChat — Docker revival (Mode A)

yChat is a legacy (2007) C++ HTTP chat server. This revival builds it **entirely
in a Docker container** and deploys it to the f3s k3s cluster as an
in-memory guest chat — **no database, no persistent user store**.

## Build

Multi-stage `Dockerfile`: Rocky Linux 9 builder (GCC 11 tolerates the legacy
C++ this tree uses) + slim Rocky 9 runtime. All optional features are OFF:

```
podman build -t ychat:dev .
podman run --rm -p 2000:2000 ychat:dev
# smoke test (ychat speaks HTTP/0.9-style responses, hence --http0.9):
curl --http0.9 http://127.0.0.1:2000/index.html
```

## Legacy-C++ patches applied

The tree does not build on a modern toolchain unmodified. Three minimal,
semantics-preserving patches were made so it builds on Rocky 9 / GCC 11:

- `src/glob.h`, `src/modl.cpp`: renamed the project's `typedef int function(...)`
  type to `mod_func_t` — it collided with `std::function` brought in by
  `using namespace std;` under C++11.
- `src/logd.cpp`: `ofstream == NULL` → `!ofstream.is_open()` (streams are no
  longer comparable to `NULL`).
- `src/sock/sock.cpp`: reordered `i_server_sock = i_sock;` to *before* the
  `set_nonblock(i_server_sock)` call — previously `set_nonblock` ran on an
  uninitialised member, returning EBADF and aborting startup.

## Runtime layout (WORKDIR /app)

| Path | Purpose | Writable? |
|------|---------|-----------|
| `bin/ychat` | server binary | no |
| `etc/ychat.conf` | config (found via `./etc/` search path) | no |
| `html/` | templates served over HTTP | no |
| `mods/{commands,html}/*.so` | runtime-loadable modules | no |
| `log/` | access/system/room logs | yes (emptyDir in k8s) |

With MySQL disabled, all user/session/room state is **in-memory only** and is
lost on restart. `chat.enableguest=true` lets guests log in without a DB.

> Note: ychat emits raw HTTP/0.9-style responses (no `HTTP/1.1 200` status
> line / headers). Modern browsers and `curl` may refuse these by default;
> `curl --http0.9` works. This is a pre-existing property of the codebase, not
> introduced by this revival.

## Push to the f3s registry

```
TAG=$(git rev-parse --short HEAD)
podman tag ychat:$TAG r0.lan.buetow.org:30001/ychat:$TAG
podman tag ychat:latest r0.lan.buetow.org:30001/ychat:latest
podman push --tls-verify=false r0.lan.buetow.org:30001/ychat:$TAG
podman push --tls-verify=false r0.lan.buetow.org:30001/ychat:latest
```

## Deploy (GitOps)

Config lives in the `conf` repo (mirrored on the in-cluster git-server):

- Helm chart: `f3s/ychat/helm-chart`
- ArgoCD app: `f3s/argocd-apps/services/ychat.yaml`

The Deployment pulls `registry.lan.buetow.org:30001/ychat:<TAG>` (tag matches
`appVersion` in `Chart.yaml`). Logs go to an `emptyDir` on `/app/log` —
ephemeral by design (no app state to persist). After chart/app edits push conf:

```
git push master master && git push r0 master
```

ArgoCD auto-syncs. URLs (LAN wildcard DNS resolves via Pi-hole):

- https://ychat.f3s.lan.buetow.org
- http://ychat.f3s.buetow.org (via OpenBSD relayd frontend, if wired)

## Optional follow-ups (not in scope for Mode A)

- Port to a modern toolchain (`hash_map`→`unordered_map`, fix implicit
  returns, modernise `configure.ac`) and drop the legacy-C++ patches.
- Mode B: enable MySQL for persistent registered users — requires a
  hand-written `CREATE TABLE` schema (none ships in this repo), a MariaDB
  StatefulSet + PVC, and a Secret for `chat.database.password`.