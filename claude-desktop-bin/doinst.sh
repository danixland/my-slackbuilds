if [ -x /usr/bin/update-desktop-database ]; then
  /usr/bin/update-desktop-database -q usr/share/applications >/dev/null 2>&1
fi

if [ -x /usr/bin/update-mime-database ]; then
  /usr/bin/update-mime-database usr/share/mime >/dev/null 2>&1
fi

if [ -e usr/share/icons/hicolor/icon-theme.cache ]; then
  if [ -x /usr/bin/gtk-update-icon-cache ]; then
    /usr/bin/gtk-update-icon-cache -f -q usr/share/icons/hicolor >/dev/null 2>&1
  fi
fi

# Chromium's SUID sandbox helper must be setuid root.  Re-assert in case the
# package did not preserve the 4755 bit.
chmod 4755 usr/lib/claude-desktop/chrome-sandbox 2>/dev/null
