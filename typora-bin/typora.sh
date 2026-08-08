#!/bin/sh
# Launcher for the bundled Typora Electron binary.
#
# Under Wayland, Electron still defaults to XWayland unless told otherwise,
# which costs sharp HiDPI scaling and native input methods.  Auto-enable the
# Ozone Wayland backend when a Wayland session is detected, and stay on the
# default (X11) path otherwise, so one package works in both sessions.
#
# Override either way with TYPORA_OZONE=wayland or TYPORA_OZONE=x11.

# @APPDIR@ is substituted by the SlackBuild at build time (it carries the
# package version, so it must not be hardcoded here).
APPDIR=@APPDIR@

ozone=${TYPORA_OZONE:-auto}
if [ "$ozone" = auto ]; then
  if [ -n "$WAYLAND_DISPLAY" ]; then
    ozone=wayland
  else
    ozone=x11
  fi
fi

if [ "$ozone" = wayland ]; then
  exec "$APPDIR/Typora" --enable-features=UseOzonePlatform \
    --ozone-platform=wayland "$@"
fi

exec "$APPDIR/Typora" "$@"
