# X11 on the RTX 5060

Two changes are needed to drive the NVIDIA card. Either one alone fails with
`Failed to acquire modesetting permission` followed by `no screens found`.

## 1. Xorg must run as root

```sh
apt install xserver-xorg-legacy
```

`/etc/X11/Xwrapper.config`:

```
allowed_users=console
needs_root_rights=yes
```

## 2. X must run on a VT other than the login VT

```sh
startx ~/.config/X11/xinitrc -- vt7
```

Login is tty1 and gettys hold tty1-tty6, so vt7 is free. Any unoccupied VT works
as long as it is not the one you logged in on. Wired into `bash/bashrc`.

## Why both are needed

Xorg logs `controlling tty is VT number N, auto-enabling KeepTty` when the target
VT matches the VT it was launched from. KeepTty switches on the server's libseat
integration, and under libseat logind hands X its DRM fd rather than X taking
master itself. The NVIDIA driver acquires modeset ownership directly, so it fails.
Targeting a different VT avoids KeepTty entirely.

Avoiding KeepTty then means the server opens the target VT itself, and `/dev/tty7`
is `root:tty` mode 0620 while only the login VT gets chowned to the user. Hence
root.

Acquiring DRM master needs no privilege at all, so that was never the problem. The
Intel iGPU never hit any of this because `modesetting` accepts a logind fd happily.

## Do not

- **No `/etc/X11/xorg.conf`.** Pinning `BusID` triggers a busid lookup the NVIDIA
  driver does not serve, failing with
  `Failed to open DRM device for pci:0000:01:00.0: -2`. Autoconfiguration is right.
- **Ignore `X11/xserverrc`.** `startx` reads only `$HOME/.xserverrc`, never the XDG
  path, so the file here is dead. Stripping `-keeptty` in it is futile anyway,
  since the server re-adds it.
- **No `nvidia-drm fbdev=0`.** Unrelated to this, and it drops the console to
  efifb at lower resolution.

## Prerequisites

- `nvidia_drm modeset=1`, mandatory for the open kernel module on Blackwell.
- `nvidia-driver-libs:i386` for 32-bit games under Proton.
