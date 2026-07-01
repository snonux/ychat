# ycurses — Docker build verification
#
# ycurses is a standalone ncurses C++ toolkit (windows/menus/attributes/
# color), demoed by an interactive full-screen menu (src/main.cpp) - there
# is no server/service to run, so unlike ../yhttpd this image is a build
# check, not a deployable runtime.
#
# Builds entirely in a container on Rocky Linux 9, whose GCC 11 accepts the
# legacy C++ this ~2005 codebase uses once the fixes below are applied.

FROM rockylinux:9 AS builder

RUN dnf -y install \
        gcc-c++ \
        make \
        perl \
        which \
        ncurses-devel \
    && dnf clean all

WORKDIR /build/ycurses
COPY . .
RUN rm -rf obj bin backuped g++.version make.version src/Makefile src/includes.add src/libs.add

# config.pl is interactive (asks yes/no); "yes" = use default before-compile
# options.
RUN echo yes | ./configure \
    && make

ENTRYPOINT ["/build/ycurses/bin"]
