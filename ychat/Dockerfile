# yChat revival image — Mode A (no DB, no SSL, no readline)
# Multi-stage build on Rocky Linux 9, whose GCC 11 still tolerates the
# legacy C++ this 2007 codebase uses (__gnu_cxx::hash_map, ofstream == NULL),
# so the tree builds with ZERO source patches.
#
# Runtime layout (WORKDIR /app):
#   bin/ychat          server binary
#   etc/ychat.conf     config (found via ./etc/ search path)
#   html/              templates (read-only, served over HTTP)
#   mods/commands/*.so runtime-loadable command modules
#   mods/html/*.so     runtime-loadable html modules
#   log/               writable logs (mount a volume here)

# ---------- builder ----------
FROM rockylinux:9 AS builder

RUN dnf -y install \
        gcc-c++ \
        make \
        autoconf \
        automake \
        libevent-devel \
    && dnf clean all

WORKDIR /build/ychat
COPY . .

# Configure with all optional features OFF (Mode A: in-memory guest chat).
# Run the top-level "all" (build modules base) so runtime .so modules are
# produced under /build/ychat/mods/{commands,html}/.
RUN cd src \
 && ./configure --disable-readline --disable-ssl --disable-mysql \
 && cd .. \
 && make -j"$(nproc)"

# ---------- runtime ----------
FROM rockylinux:9 AS runtime

RUN dnf -y install \
        libevent \
        libstdc++ \
        tzdata \
        ca-certificates \
    && dnf clean all

# Non-root runtime user. ychat binds port 2000 (unprivileged).
RUN useradd -r -u 1000 -d /app -s /sbin/nologin ychat

WORKDIR /app

# Binary
COPY --from=builder /build/ychat/bin/ychat /app/bin/ychat

# Read-only resources
COPY --from=builder /build/ychat/html/ /app/html/
COPY --from=builder /build/ychat/mods/ /app/mods/
COPY etc/ychat.conf /app/etc/ychat.conf

# Writable log dir
RUN mkdir -p /app/log/rooms && chown -R ychat:ychat /app

USER 1000:1000
EXPOSE 2000

ENTRYPOINT ["/app/bin/ychat"]