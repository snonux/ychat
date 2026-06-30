#!/bin/sh
# yChat container entrypoint.
# Ensure writable log subdirs exist (an emptyDir mounted on /app/log hides
# the image's pre-created /app/log/rooms, so recreate it here). yChat exits
# the whole process if a log file can't be opened, so this is required for
# room logs (chat.logging.roomlogdir = log/rooms/) to work.
set -e
mkdir -p /app/log/rooms
exec "$@"