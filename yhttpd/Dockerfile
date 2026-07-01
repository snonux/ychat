# yhttpd — Docker build (revival)
#
# yhttpd is a minimal httpd derived from ychat's socket/engine. This image
# builds it entirely in a container on Rocky Linux 9 (its bespoke perl build
# system was gated to g++ 3.x; the configure now accepts any GNU g++).
#
# NCURSES and CLI are disabled (glob.h) so the build needs no ncurses and
# produces a pure httpd. The server listens on port 2000 (httpd.serverport).
#
# STATUS: with the ported fixes yhttpd now BUILDS on a modern toolchain and
# serves requests without crashing when built+run here (Rocky Linux 9 /
# GCC 11). A SIGILL heap-corruption crash in sock::_close DOES reproduce
# when built on a newer host toolchain (GCC 16) - see yhttpd/DOCKER.md for
# why this doesn't mean it's fixed. Concurrent bursts still see some
# connection resets (not crashes). See yhttpd/DOCKER.md for details.

# ---------- builder ----------
FROM rockylinux:9 AS builder

RUN dnf -y install \
        gcc-c++ \
        make \
        perl \
        which \
        libevent-devel \
    && dnf clean all

WORKDIR /build/yhttpd
COPY . .
RUN rm -rf obj bin backuped g++.version make.version src/Makefile src/includes.add src/libs.add

# config.pl is interactive (asks yes/no); "yes" = use default before-compile
# options (keeps our glob.h: NCURSES/CLI disabled). src/configure scans glob.h
# and drops the ncur/ + cli/ sources from the build accordingly.
RUN echo yes | ./configure \
    && make

# ---------- runtime ----------
FROM rockylinux:9 AS runtime

RUN dnf -y install libevent libstdc++ tzdata ca-certificates \
    && dnf clean all

RUN useradd -r -u 1000 -d /app -s /sbin/nologin yhttpd

WORKDIR /app
COPY --from=builder /build/yhttpd/bin /app/bin/yhttpd
COPY --from=builder /build/yhttpd/etc/ /app/etc/
COPY --from=builder /build/yhttpd/html/ /app/html/

RUN mkdir -p /app/log && chown -R yhttpd:yhttpd /app

USER 1000:1000
EXPOSE 2000

ENTRYPOINT ["/app/bin/yhttpd"]