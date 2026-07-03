# AGENTS.md

Guidance for coding agents working in this repository.

## Repository

Three small legacy C++ projects sharing a socket/event/template engine:
`ychat/` (HTTP web chat server), `yhttpd/` (tiny standalone HTTP server), and
`ycurses/` (curses front-end demo). See [README.md](README.md) for the
overview and local Docker/podman quickstart, and
[ychat/DOCKER.md](ychat/DOCKER.md) for the detailed ychat build notes.

## Building and running

Everything builds **inside Docker/podman** — no host toolchain required. Use
the multi-stage `Dockerfile` in each subproject. For local build-and-run
steps, follow [README.md](README.md) and the per-subproject `DOCKER.md` /
`BUILD.md` files.

## Deployment (homelab-only, not in this repo)

Deploying ychat to the f3s homelab k3s cluster (private registry push, Helm
chart, ArgoCD Application, PVC for `/app/data`) is **homelab-specific and
deliberately not documented in this public repo**.

If you are running with the private `f3s` Amp skill available, its
`references/ychat.md` reference has the full deployment runbook (image
build/push, Helm chart path, ArgoCD app, storage/PVC notes). Load the `f3s`
skill for those details. Without that skill, treat deployment as out of scope.
