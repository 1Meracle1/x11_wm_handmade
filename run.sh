#!/bin/fish

set XEPHYR $(whereis -b Xephyr | sed -E 's/^.*: ?//')
if [ -z "$XEPHYR" ]
  echo "Xephyr not found"
  exit 1
end

xinit ./xinitrc -- "$XEPHYR" :100 -ac -screen 1920x1080 -host-cursor
