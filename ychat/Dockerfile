# yChat revival image — embedded SQLite backend (no MySQL server, no SSL, no
# readline).
#
# Multi-stage build on Rocky Linux 9, whose GCC 11 still tolerates the
# legacy C++ this 2007 codebase uses (__gnu_cxx::hash_map, ofstream == NULL),
# so the tree builds with ZERO source patches beyond the ones already applied
# during this revival (see git log / README.md).
#
# A database is no longer optional: ./configure always requires sqlite3.h +
# libsqlite3 (see src/configure.ac), so DATABASE is unconditionally compiled
# in — the old "Mode A" in-memory-only, no-account guest chat no longer
# exists as a build option. Registration and login persist across restarts
# as long as /app/data is on a persistent volume. Unregistered guest logins
# are still supported as a *runtime* toggle (chat.enableguest=true in
# etc/ychat.conf) — that's independent of the database being compiled in.
#
# Runtime layout (WORKDIR /app):
#   bin/ychat          server binary
#   etc/ychat.conf     config (found via ./etc/ search path)
#   html/              templates, including register.html/options.html
#   mods/commands/*.so runtime-loadable command modules
#   mods/html/*.so     runtime-loadable html modules
#   log/               writable logs (mount a volume here)
#   data/              writable SQLite db file (mount a volume here for
#                       persistence across container restarts)

# ---------- builder ----------
FROM rockylinux:9 AS builder

RUN dnf -y install \
        gcc-c++ \
        make \
        autoconf \
        automake \
        libevent-devel \
        sqlite-devel \
    && dnf clean all

WORKDIR /build/ychat
COPY . .

# Configure with SSL/MySQL/readline off; SQLite is required and always on
# (no --enable-sqlite flag any more — see src/configure.ac). Run the
# top-level "all" (build modules base) so runtime .so modules are produced
# under /build/ychat/mods/{commands,html}/.
RUN cd src \
 && ./configure --disable-readline --disable-ssl \
 && cd .. \
 && make -j"$(nproc)"

# ---------- runtime ----------
FROM rockylinux:9 AS runtime

RUN dnf -y install \
        libevent \
        libstdc++ \
        sqlite-libs \
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
# Defense-in-depth: the /exec command module does popen() on an attacker-
# controlled shell string (host RCE for any operator). It is gated to
# operators by permissions and the defaultop grant now requires a
# registered (database-authenticated) user, but we also physically omit it
# from the image so the RCE primitive is absent, not merely unreachable.
RUN rm -f /app/mods/commands/yc_exec.so
COPY docker-entrypoint.sh /app/docker-entrypoint.sh
COPY etc/ychat.conf /app/etc/ychat.conf

# Writable log + data dirs (entrypoint recreates log/rooms since a volume
# mount on /app/log hides the image's copy; data/ holds the sqlite file).
RUN mkdir -p /app/log/rooms /app/data && chown -R ychat:ychat /app

USER 1000:1000
EXPOSE 2000

ENTRYPOINT ["/app/docker-entrypoint.sh"]
# chat.session.md5hash=false: the md5 session-id path does a bad substr()
# (the default salt contains chars not in chat.session.validchars, so
# s_ret.find(salt) returns npos and the subsequent substr/append corrupts
# the heap and segfaults on login). The 32-char random id is enough.
# chat.database.dbname=data/ychat.db: SQLite file path (see con.cpp/con.h —
# this config key is reused as a file path instead of a MySQL db name when
# built against SQLite, which is the only backend this image builds).
CMD ["/app/bin/ychat", "-o", "chat.session.md5hash", "false", "-o", "chat.database.dbname", "data/ychat.db"]
