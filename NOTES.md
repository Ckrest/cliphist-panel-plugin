# Cliphist Panel Plugin - Systems Integration Notes

## Path Dependencies

**Install location:** `/usr/lib/x86_64-linux-gnu/xfce4/panel/plugins/`
- Plugin library: `libcliphist-panel-plugin.so`
- Desktop entry: `/usr/share/xfce4/panel/plugins/cliphist-panel-plugin.desktop`
- Icon: `/usr/share/pixmaps/cliphist-icon.svg`

These paths are hardcoded in the Makefile. If moving the project, update the Makefile install targets.

## Dependencies

- `cliphist` - Must be running (typically via systemd user service)
- `wl-copy` - Used for copying selected entries back to clipboard
- `xfce4-panel` - The panel itself

## Panel Management

When using systemd to manage xfce4-panel:
- **Restart:** `systemctl --user restart xfce4-panel`
- **Do NOT use:** `xfce4-panel --restart` (causes restart loops with systemd)

The systemd service has `Restart=always`, so manual `--restart` conflicts with systemd's auto-restart.

## Configuration

No user configuration needed. Entry count and image size are compile-time constants in:
- `cliphist-client.c`: `MAX_ENTRIES` (currently 6)
- `cliphist-button.c`: Image preview size (300x200px max)

## Build Notes

Standard make workflow:
```bash
make clean && make
sudo -A make install
systemctl --user restart xfce4-panel
```

Uses `sudo -A` to invoke graphical askpass helper for password entry.

## Known Issues

- CLI pasting in terminal applications may not work correctly - needs investigation of wl-copy interaction with terminal clipboard selection
