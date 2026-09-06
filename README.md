# idler

A lightweight auto-clicker for Windows 11 and macOS. Set an interval, pick a stop condition, and let it click.

## Features

- Click every N seconds (supports fractions, e.g. 0.25 s)
- Three modes: infinite, stop after a duration, or stop after N clicks
- Live status showing click count and time/clicks remaining
- No installer required — portable on both platforms

## Usage

1. Set **Click every** to your desired interval in seconds.
2. Choose a mode:
   - **Infinite** — clicks until you press Stop.
   - **Stop after duration** — clicks for the specified number of seconds, then stops automatically.
   - **Stop after N clicks** — fires exactly N clicks, then stops automatically.
3. Press **Start**. The status line updates live with click count and remaining time or clicks.
4. Press **Stop** at any time to abort.

The interval and mode controls are locked while running and re-enabled after stopping.

## Building

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

Run the included batch script:

```bat
build_portable_windows.bat
```

Output is in `build\portable\`. Copy that folder to any Windows 11 machine and run `idler.exe` — no install needed. Qt DLLs are bundled automatically via `windeployqt`.

**Single .exe (no DLLs):** requires a statically compiled Qt build, then:

```bat
cmake -B build\static -DCMAKE_BUILD_TYPE=Release -DPORTABLE=ON -DCMAKE_PREFIX_PATH=C:\Qt\static
cmake --build build\static --config Release
```

### macOS — .app bundle and .dmg

```bash
./build_portable_mac.sh
```

Produces:
- `build/mac/idler.app` — drag to `/Applications` or double-click to run
- `build/idler.dmg` — disk image ready to share

Qt frameworks are embedded in the bundle via `macdeployqt`, so no Qt installation is needed on the target machine.

### Manual CMake build (either platform)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/Qt6
cmake --build build --config Release
```

## Platform notes

### macOS — Accessibility permission

`CGEventPost` (the API used to synthesize clicks) requires Accessibility access. On first launch, go to:

**System Settings → Privacy & Security → Accessibility → enable idler**

Without this, the app opens normally but clicks are silently blocked by the OS.

### macOS — Gatekeeper

If you share the `.dmg`, recipients will see a warning because the app is unsigned. They can bypass it by right-clicking the app and choosing **Open** the first time. Removing the warning permanently requires an Apple Developer account for code signing and notarization.

### Windows — antivirus

Some antivirus tools flag auto-clickers due to how `SendInput` is used. If Windows Defender blocks the exe, add an exclusion for the `build\portable\` folder.

## License

MIT
