# idler

A lightweight auto-clicker for Windows 11, macOS, and Linux. Set an interval, pick a stop condition, and let it click.

## Features

- Click every N seconds (supports fractions, e.g. 0.25 s)
- Three modes: infinite, stop after a duration, or stop after N clicks
- Live status showing click count and time/clicks remaining
- No installer required — portable on Windows and macOS

---

## How to use

1. Set **Click every** to your desired interval and choose the unit — **seconds**, **minutes**, or **hours**.
2. Choose a mode:
   - **Infinite** — clicks until you press Stop.
   - **Stop after** (duration) — enter a value and unit (seconds/minutes/hours); stops automatically when the time is up.
   - **Stop after** (N clicks) — fires exactly N clicks, then stops automatically.
3. Press **Start**. The status line updates live with click count and remaining time or clicks.
4. Press **Stop** at any time to abort.

The interval, unit selectors, and mode controls are locked while running and re-enabled after stopping.

---

## Installing

### Windows

1. Download the latest `idler-windows.zip` from [Releases](https://github.com/ikjbi/idler/releases).
2. Extract the zip anywhere (e.g. `C:\Tools\idler\`).
3. Run `idler.exe` — no install needed, all required DLLs are included.

> **Antivirus:** Some antivirus tools flag auto-clickers due to how `SendInput` is used. If Windows Defender blocks the exe, add an exclusion for the folder.

---

### macOS

1. Download the latest `idler.dmg` from [Releases](https://github.com/ikjbi/idler/releases).
2. Open the `.dmg` and drag **idler.app** to your **Applications** folder.
3. Launch idler from Applications or Spotlight.

**First launch — Gatekeeper warning:**  
Because the app is unsigned, macOS will say it "can't be opened because the developer is unverified." To bypass it:
- Right-click (or Control-click) `idler.app` → **Open** → **Open** again in the dialog.

You only need to do this once.

**Accessibility permission (required):**  
idler uses `CGEventPost` to synthesize clicks. macOS blocks this by default. On first run you will be prompted, or go manually to:

**System Settings → Privacy & Security → Accessibility → enable idler**

Without this, the app opens normally but clicks are silently blocked.

---

### Linux

idler uses the X11 XTest extension to send clicks. Wayland is supported via XWayland (which most desktop environments enable by default).

#### Install dependencies

**Debian / Ubuntu / Linux Mint:**
```bash
sudo apt install cmake qt6-base-dev libxtst-dev libx11-dev
```

**Fedora / RHEL:**
```bash
sudo dnf install cmake qt6-qtbase-devel libXtst-devel libX11-devel
```

**Arch Linux:**
```bash
sudo pacman -S cmake qt6-base libxtst
```

#### Build and install

```bash
git clone https://github.com/ikjbi/idler.git
cd idler
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build   # installs to /usr/local/bin/idler
```

Or run it directly without installing:
```bash
./build/idler
```

> **Wayland note:** If clicks don't register, your compositor may have XWayland disabled. Set `QT_QPA_PLATFORM=xcb` before launching: `QT_QPA_PLATFORM=xcb ./build/idler`

---

## Building from source (Windows & macOS)

### Requirements

| Platform | Tools |
|---|---|
| Windows | Qt 6, CMake 3.16+, MSVC 2019+ or MinGW |
| macOS | Qt 6, CMake 3.16+, Xcode Command Line Tools |

Install Qt via [qt.io](https://www.qt.io/download) or on macOS with Homebrew:

```bash
brew install qt cmake
```

### Windows — portable folder

```bat
build_portable_windows.bat
```

Output is in `build\portable\`. Copy that folder to any Windows 11 machine and run `idler.exe`.

**Single .exe (no DLLs):** requires a statically compiled Qt, then:

```bat
cmake -B build\static -DCMAKE_BUILD_TYPE=Release -DPORTABLE=ON -DCMAKE_PREFIX_PATH=C:\Qt\static
cmake --build build\static --config Release
```

### macOS — .app bundle and .dmg

```bash
./build_portable_mac.sh
```

Produces `build/mac/idler.app` and `build/idler.dmg`. Qt frameworks are embedded so no Qt installation is needed on the target machine.

---

## License

MIT
