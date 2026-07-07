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

Deploying ychat to a homelab cluster (private registry push, Helm chart,
ArgoCD Application, PVC for `/app/data`) is **homelab-specific and out of
scope for this public repo**. The full deployment runbook lives in the
private homelab skill (its `references/ychat.md`). Load that skill for those
details; without it, treat deployment as out of scope.
