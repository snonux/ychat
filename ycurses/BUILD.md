# ycurses — build revival

`ycurses` is a standalone ncurses C++ toolkit (windows, menus, attributes,
color) from the same ~2003-2005 era as `../ychat` and `../yhttpd`, but it
shares no source files with them (no `sock`/`reqp`/`html`/`logd`/`tool`) -
it's a UI library, not a network daemon. `src/main.cpp` is an interactive
full-screen demo menu; there is no service to deploy, so unlike `../yhttpd`
this only needed a build fix, verified in Docker (`Dockerfile` here is a
build check, not a runtime image).

## Build & verify (local)

```
podman build -t ycurses:dev .
podman run --rm -it ycurses:dev   # interactive demo menu; Ctrl-C to exit
```

Verified: builds clean (one pre-existing, harmless `-Wreturn-type` warning
in `menu.cpp`), links, and runs - initializes curses, draws the demo screen
using `color`/`attributes` (exercising the fix below), exits cleanly on
EOF/SIGINT.

## What was fixed

- **Top-level `configure`'s g++ 3.x version gate** now accepts any GNU g++
  (was hardcoded to versions 3.1-3.4), same fix as `../yhttpd/configure`.
- **`scripts/config.pl` silently failed on modern Perl**: it does `use
  scripts::modules::file`, which needs `.` on `@INC`; Perl 5.26+ dropped
  `.` from the default `@INC`. The top-level `configure` invoked it via
  `system("perl ...")` without `-I.`, so it `BEGIN`-failed before ever
  reading the "yes" answer - harmless here only because the committed
  `src/glob.h` defaults already matched, but it means the interactive
  configurator never actually ran. Fixed with `perl -I.` (also applied to
  `../yhttpd/configure`, which has the same bug).
- **`src/configure`'s library search path list predates 64-bit multilib
  distros**: it only checked `/usr/lib`, `/lib`, etc., never `/usr/lib64`
  or `/lib64`, so `libpanel.so`/`libmenu.so`/`libncurses.so` were reported
  "NOT OK" on Rocky Linux 9 even though they're installed - the script
  aborts before generating a Makefile if a required lib "isn't found" (the
  actual compiler/linker would have found them fine via its own default
  search path; this is purely the custom dependency-checker being
  outdated). Added `/lib64`, `/usr/lib64`, `/usr/local/lib64` to
  `@libpaths` (also applied to `../yhttpd/src/configure` for the same
  latent gap, even though yhttpd doesn't currently need any of these
  libs).
- **`attributes` class member functions named `set`/`set(int)` collide
  with `std::set`**: `attributes.h` does `using namespace std` and declares
  `set<int> set_attr` (a `std::set`), then separately declares member
  *functions* literally named `set` - GCC 11 treats declaring a member
  with the same name as a using-directive-visible type as ill-formed
  ("changes meaning of 'set'"), not just a warning. Renamed both
  overloads to `set_attr_flag` (`attributes.h`, `attributes.cpp`); no
  external caller used the bare `set(...)`/`set(int)` names (checked all
  of `src/`), only the named setters (`set_bold`, etc.) and the unrelated
  `window::set_attributes`. Same category of fix as the
  `function`->`mod_func_t` rename in `../ychat`/`../yhttpd`'s `glob.h`.

## Known pre-existing issue (not fixed - out of scope for this pass)

`attributes::set_attr_flag(bool b, int i_attr)` (`attributes.cpp`) never
actually removes a flag when `b` is `false`: it only checks whether the
current membership already equals `b` and, if not, unconditionally
**inserts** into `set_attr` - so `unset(...)` cannot clear an attribute
once set (it either no-ops or wrongly re-inserts). This bug predates this
revival and is unrelated to the build/toolchain fixes above; flagging it
here rather than fixing it silently while touching this function.
