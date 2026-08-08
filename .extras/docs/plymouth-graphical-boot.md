# Graphical boot on Slackware with Plymouth

Notes for replacing the plain-text boot wall with a graphical splash on this
machine. Research only, nothing has been installed or changed yet.

Written 2026-08-07. Everything under "Current system state" was measured on
that date, on the running system, not assumed. Re-verify before acting if the
kernel or hardware changed.

---

## Conclusions up front

**Do not patch the kernel.** Distributions stopped patching kernels for boot
splash around 2009. The old `bootsplash` and `fbsplash` patch sets are dead,
and the in-kernel `bootsplash` revival (Max Staudt, ~2017) never reached
mainline. `CONFIG_LOGO` still exists but only gives a 224-colour CLUT224 PPM
logo, not an animated splash.

**Everyone uses Plymouth**, a userspace daemon started from the initrd. Ubuntu,
Mint, Debian, Fedora, all the same stack, differing only in theme.

**SBo has both packages already**, so this is not a from-scratch port:

| Package | Version | Notes |
|---|---|---|
| `system/plymouth` | 22.02.122 | `--disable-systemd-integration` already set, so it suits Slackware's BSD init |
| `system/plymouth-slackware-theme` | 1.1 | Theme `slackware-logo`, based on an Arch theme, ships `/etc/plymouth/plymouthd.conf` |

Neither package touches the initrd or the init scripts. **That wiring is the
entire job** and it is all manual.

### Update 2026-08-08: this repo now ships plymouth 26.134.222

SBo's 22.02.122 is four years behind upstream and **predates SimpleDRM**, which
changes the plan substantially (see below). A local package was written at
`plymouth/` in this repo, built and lint-clean on -current. Findings:

- Upstream switched **autotools to meson**, so the SBo script could not be
  version-bumped, it had to be rewritten from the meson template.
- `systemd-integration` now defaults to **true** in meson and must be turned
  off explicitly, otherwise the build wants `systemd.pc` and
  `systemd-tty-ask-password-agent`.
- All build deps are satisfied by stock -current, so `REQUIRES=""`.
- **It does not build on 15.0**: `PANGO_ATTR_FONT_SCALE` needs pango >= 1.50,
  and 15.0 ships 1.48.11. Since 15.0 is the SBo baseline, this is not
  submittable to SBo as-is.
- The SBo theme package ships its own `/etc/plymouth/plymouthd.conf` setting
  `Theme=slackware-logo`, the same path the main package installs. Installing
  the theme after plymouth overwrites that file.

---

## How the other distributions do it

The stack is identical everywhere:

1. **Kernel cmdline** carries `quiet splash`. `quiet` suppresses printk output,
   `splash` is a flag Plymouth reads.
2. **KMS driver** loads early (in the initrd or builtin) and creates
   `/dev/dri/card*`. Plymouth's `drm.so` renderer draws on it, falling back to
   `frame-buffer.so` on `/dev/fb0`.
3. **`plymouthd` runs from the initramfs**, started before the root pivot. On
   Debian and Ubuntu an `initramfs-tools` hook at
   `/usr/share/initramfs-tools/hooks/plymouth` copies the daemon, theme and
   fonts into the image.
4. **Handoff to the display manager.** With systemd, `plymouth-start.service`
   runs `Before=display-manager.service` and `plymouth-quit.service` tears the
   splash down once the DM owns the screen. Both draw through KMS, so there is
   no mode switch and no flicker.
5. **The daemon survives `switch_root`**, which is why it is a daemon and not a
   static image. It also renders the LUKS passphrase prompt in graphics mode.

Theme selection differs cosmetically:

| Distro | Default theme | Selector |
|---|---|---|
| Ubuntu | `bgrt`, then spinner | `update-alternatives --config default.plymouth` |
| Mint | `mint-logo` (script module) | same |
| Debian | `spinner` / `moonlight` | same, then `update-initramfs -u` |
| Fedora | `bgrt` / `charge` | `plymouth-set-default-theme -R` |

`bgrt` is worth knowing about: on UEFI the firmware has already painted the
vendor logo, and the kernel exposes that exact bitmap and position through the
ACPI BGRT table. Plymouth reuses it, so the screen appears never to change
between firmware and OS. It is the reason modern Ubuntu boots look seamless.

**None of the above applies directly here.** It all assumes systemd units plus
`initramfs-tools` or `dracut`. Slackware has neither.

---

## Current system state

Measured 2026-08-07 on this machine.

| Item | Value |
|---|---|
| Slackware | 15.0+ (-current) |
| Kernel | 6.18.41 |
| Bootloader | GRUB, EFI, config at `/boot/grub/grub.cfg`, ESP on `/dev/nvme0n1p1` |
| Root | `/dev/myvg/root`, **LVM**, ext4 |
| Initrd | `/boot/initrd-6.18.41.img`, 16M, symlinked as `initrd-generic.img` |
| `/boot/initrd-tree` | **Does not exist**, initrd is built directly by `mkinitrd -c` |
| Runlevel | 4 (`id:4:initdefault:`) |
| Display manager | SDDM (`/usr/bin/sddm`), no `rc.4.local` present |
| GPU | Intel, PCI id `e20b` (Panther Lake, per `ptl_*` firmware in the module) |
| GPU driver | `xe`, **loadable module**, currently loaded, 3.7M, 56 refs |
| Plymouth | Not installed |

Current kernel command line:

```
GRUB_CMDLINE_LINUX_DEFAULT="i915.force_probe=!e20b xe.force_probe=e20b"
```

`GRUB_GFXPAYLOAD_LINUX=keep` is already set, which is what you want: the
graphics mode set by GRUB carries into the kernel.

The initrd was generated by:

```
/sbin/mkinitrd -c -k 6.18.41 -f ext4 -r /dev/myvg/root \
  -m xhci-pci:ohci-pci:ehci-pci:xhci-hcd:uhci-hcd:ehci-hcd:hid:usbhid:i2c-hid:hid_generic:hid-asus:hid-cherry:hid-logitech:hid-logitech-dj:hid-logitech-hidpp:hid-lenovo:hid-microsoft:hid_multitouch \
  -L -u -L -R -o /boot/initrd-6.18.41.img
```

That string is stored inside the image as `/command_line`, so it can always be
recovered from the initrd itself.

### The finding that matters most

**`xe.ko` is not in the initrd.** The only DRM-related module present is
`drm.ko`, pulled in as a dependency of something else. Verified by extracting
the image and listing every `.ko`:

```
asus-wmi  dm-bufio  dm-snapshot  drm  ehci-hcd  ehci-pci  hid-asus
hid-cherry  hid-generic  hid-lenovo  hid-logitech  hid-logitech-dj
hid-logitech-hidpp  hid-microsoft  hid-multitouch  hid  i2c-hid
ohci-hcd  ohci-pci  platform_profile  rfkill  sparse-keymap  uhci-hcd
usbhid  video  wmi  xhci-hcd  xhci-pci
```

`drm.ko` alone creates no `/dev/dri/card*`. Without the actual GPU driver,
Plymouth's `drm.so` renderer has nothing to draw on during the initrd phase, so
the splash cannot start until after the pivot. That is the difference between a
seamless boot and a splash that shows up halfway through, which looks worse
than no splash at all.

So adding `xe` to the initrd is a **prerequisite**, not an optimisation.

**Superseded on 2026-08-08 for plymouth 26.x.** The above holds for SBo's
22.02.122, which has no SimpleDRM support (verified: zero matches for
`simpledrm` anywhere in that tarball). Upstream added SimpleDRM in **24.004.60**,
and on UEFI systems plymouth 26.x uses it **by default** to draw on the EFI
framebuffer, so the GPU driver does not need to be in the initrd at all. This
machine is UEFI, so with the 26.134.222 package in this repo, **step 2.1 below
is no longer a prerequisite**, which also removes the firmware-bloat problem and
the per-kernel-update `xe` maintenance.

Two caveats: with SimpleDRM secondary monitors stay dark during boot (irrelevant
here, root is plain LVM with no LUKS prompt), and `plymouth.use-simpledrm=0`
disables it if it misbehaves.

---

## Plan

Two stages. Stage one is reversible in seconds and may be all that is wanted.

### Stage 1: silence the text wall (no Plymouth, no initrd work)

Append `quiet loglevel=3` to the existing cmdline, keeping the force_probe
flags:

```
GRUB_CMDLINE_LINUX_DEFAULT="i915.force_probe=!e20b xe.force_probe=e20b quiet loglevel=3"
```

Then regenerate:

```
grub-mkconfig -o /boot/grub/grub.cfg
```

This alone removes the scrolling kernel messages. No packages, no initrd
changes, nothing that can prevent a boot. Optionally add
`vt.global_cursor_default=0` to suppress the blinking cursor.

**If this is enough, stop here.**

### Stage 2: actual Plymouth splash

Ordered by dependency. Do not skip step 1.

#### 2.1 Get `xe` into the initrd, and confirm it still boots

Rebuild with `xe` added to the module list, keeping every existing flag from
`/command_line`:

```
/sbin/mkinitrd -c -k 6.18.41 -f ext4 -r /dev/myvg/root \
  -m xhci-pci:ohci-pci:ehci-pci:xhci-hcd:uhci-hcd:ehci-hcd:hid:usbhid:i2c-hid:hid_generic:hid-asus:hid-cherry:hid-logitech:hid-logitech-dj:hid-logitech-hidpp:hid-lenovo:hid-microsoft:hid_multitouch:xe \
  -L -u -L -R -o /boot/initrd-plymouth.img
```

Note the **different output name**. Keep `initrd-6.18.41.img` untouched as the
known-good fallback, and add a second GRUB entry pointing at the new image. See
"Safety" below.

Firmware is the catch. `xe` declares 39 firmware files, and `/lib/firmware/xe`
is 5.5M (`/lib/firmware/i915` is another 27M, and the `xe` module references
`i915/*` blobs too for older parts). For this GPU only the `ptl_*` files should
be needed:

```
xe/ptl_guc_70.bin
xe/ptl_huc.bin
```

`mkinitrd -u` pulls firmware automatically, but verify after building that
those files are actually inside the image. Missing GuC/HuC firmware makes the
driver fail or fall back, and the splash goes with it.

Boot this and confirm the system still comes up before going further.

#### 2.2 Install the packages

```
sbopkg -i plymouth
sbopkg -i plymouth-slackware-theme
```

The theme drops `/usr/share/plymouth/themes/slackware-logo/` and
`/etc/plymouth/plymouthd.conf` containing:

```
[Daemon]
Theme=slackware-logo
```

#### 2.3 Populate the initrd with Plymouth

Upstream ships `plymouth-populate-initrd`, but it is written for
dracut/initramfs-tools layouts and will not slot into Slackware's initrd-tree
cleanly. Expect to copy by hand.

Required inside the image:

- `plymouthd` and the `plymouth` client binary
- `/usr/lib64/plymouth/renderers/drm.so` and `frame-buffer.so`
- `/usr/share/plymouth/themes/slackware-logo/` in full
- `/etc/plymouth/plymouthd.conf`
- the label plugin **and its fonts**, if the theme draws any text
- every shared library the above link against (check with `ldd`)

The font omission is the classic failure: the theme renders blank or partially,
and it is not obvious why.

Since `/boot/initrd-tree` does not exist, either create one with
`mkinitrd -s`, add the files there and rebuild from it, or unpack the generated
image, add the files, and repack. The tree approach is more repeatable.

#### 2.4 Start the daemon inside the initrd

`/init` in the initrd is `#!/bin/ash`. Module loading happens through
`/load_kernel_modules`, which is a plain list of `modprobe -v` lines.

After the DRM module is up and before any LUKS prompt, add:

```sh
plymouthd --mode=boot --attach-to-session
plymouth show-splash
```

Ordering matters: `xe` must already be loaded, otherwise there is no
`/dev/dri/card0` to render to.

#### 2.5 Kernel cmdline

```
GRUB_CMDLINE_LINUX_DEFAULT="i915.force_probe=!e20b xe.force_probe=e20b quiet splash"
```

`splash` is the flag Plymouth looks for. Keep `GRUB_GFXPAYLOAD_LINUX=keep`.

#### 2.6 Quit the splash at the right moment

There is no systemd, so nothing tears the splash down automatically. Runlevel
is 4 and the display manager is SDDM, started from the end of `/etc/rc.d/rc.4`.

`rc.4` `exec`s the DM, so anything appended after that never runs. The call has
to go **before** the `exec` chain, or into `/etc/rc.d/rc.4.local`, which `rc.4`
checks for first and which does not currently exist. `rc.4.local` is the
cleaner hook because it leaves the stock script untouched, but note it also
`exec`s, so it would have to start SDDM itself.

```sh
/usr/bin/plymouth quit --retain-splash
```

`--retain-splash` leaves the last frame on screen so SDDM can fade in over it
rather than flashing a bare console. For a console-only boot, call
`plymouth quit` at the end of `rc.M` instead.

---

## Do the boot messages stay reachable?

Yes. This is what Debian and Mint do, and it is not a systemd feature.
Pressing **Esc** during boot switches from the splash to the boot text.
Plymouth also does its own console redirection and logs messages to
`/var/log/boot.log`, independent of any init system, which is why
`plymouth.nolog` is documented as disabling "logging and console redirection".

Slackware caveat: `rc.S` and `rc.M` are not plymouth-aware the way Debian's
systemd units are, so there are no `plymouth message` progress updates tied to
boot stages. Whether the rc output ends up behind the splash depends on
plymouthd surviving `switch_root` and holding the console, see below. Stage 1
alone (`quiet loglevel=3`) does **not** silence the rc scripts, it only
suppresses kernel printk, which is why the wall is still there after stage 1.

## Known problem areas

**`switch_root` continuity.** This is the most likely thing to consume time.
`plymouthd` holds the VT, and its socket lives in `/run`. The daemon must not be
killed when the initramfs is torn down, and the real root needs a matching
`/run` for the client to reconnect. Get it wrong and the splash dies at the
pivot, dropping to a text wall for the second half of boot. Note that `/run` in
the current initrd is an empty directory.

**LVM plus splash ordering.** Root is on `/dev/myvg/root`, so LVM activation
happens in the initrd. Plymouth should be started before that, so the splash
covers the LVM and filesystem phase rather than appearing after it.

**Firmware bloat.** Pulling all of `/lib/firmware/xe` plus the `i915` blobs the
module references would add a lot to a 16M initrd. Restrict to the `ptl_*`
files needed by this GPU.

**Kernel updates.** Every kernel bump means rebuilding the initrd, and the
Plymouth files have to be re-added each time unless the initrd-tree is kept
around. This is ongoing maintenance, and the main argument for stopping at
stage 1.

**`plymouth-populate-initrd` will not work unmodified.** It assumes a
dracut/initramfs-tools layout.

---

## Safety

The initrd is on the boot path. A broken one means no boot, and repair requires
install media.

Before any stage 2 work:

1. Build to a **new filename** (`initrd-plymouth.img`), never overwrite
   `initrd-6.18.41.img`.
2. Add a **second GRUB menu entry** pointing at the new initrd, keeping the
   working entry as the default until the new one is proven.
3. Keep install media reachable.

`GRUB_DEFAULT=0` and `GRUB_TIMEOUT=10` currently, so there is a visible menu to
choose from, which is what makes the fallback entry usable.

---

## References

- Both SBo READMEs point at an LQ thread for Slackware-specific instructions:
  <https://www.linuxquestions.org/questions/slackware-14/bootsplash-4175742241/>
  It is still not readable (LQ returns HTTP 403 to automated fetches, a
  browser User-Agent does not help), so **it has not been incorporated here**.
  It is also an old thread, so its value is now doubtful.
- <https://wiki.archlinux.org/title/Plymouth> is the better reference and was
  read on 2026-08-08. It is the source for the SimpleDRM behaviour, the Esc
  toggle, and the `plymouth.nolog` / `plymouth.debug` / `plymouth.enable=0`
  flags. Note it 403s to plain fetches too (Anubis); a browser User-Agent
  works, or `?action=raw` for the wikitext.
- Theme upstream: <https://github.com/murkl/plymouth-theme-arch-os>
- Plymouth upstream:
  <https://www.freedesktop.org/wiki/Software/Plymouth/>

---

## Status

Updated 2026-08-08.

**Stage 1 is done.** `quiet loglevel=3` is on the kernel command line and
`grub-mkconfig` has been run. Verified after reboot: the kernel printk wall is
gone, but **the Slackware rc script output is not**, which is expected, no
kernel flag suppresses it. That is what motivates stage 2.

**A plymouth 26.134.222 package exists** at `plymouth/` in this repo. It builds
and passes `sbopkglint` on -current, and fails on 15.0 (pango too old). It is
not installed on this machine yet.

**Stage 2 is otherwise unstarted**, but it is now cheaper than described below:
step 2.1 (`xe` in the initrd) is **no longer a prerequisite** thanks to
SimpleDRM, so the remaining work is initrd population (2.3), starting the
daemon (2.4), the cmdline (2.5), and the quit hook (2.6).

Next concrete step, and the cheapest way to derisk everything: install the
package and preview the theme from a console, no reboot and no initrd work
needed:

```
plymouthd; plymouth --show-splash; sleep 5; plymouth --quit
```

That confirms the renderer works on this GPU before any boot-path change.
