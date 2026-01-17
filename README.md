# Cliphist Panel Plugin

XFCE panel plugin for [cliphist](https://github.com/sentriz/cliphist) clipboard history.

## Features

- Panel button with clipboard icon
- Click to show last 6 clipboard entries
- Select entry to copy to clipboard
- Image preview support (thumbnails up to 300x200px)
- Automatic image detection and display
- Integrates with cliphist daemon

## Dependencies

- cliphist
- wl-clipboard
- libxfce4panel-2.0-dev
- libgtk-3-dev
- libxfce4util-dev

## Building

```bash
make
sudo -A make install
```

## Installation

After building and installing:

1. Restart the XFCE panel:
   - If using systemd: `systemctl --user restart xfce4-panel`
   - Otherwise: `xfce4-panel --restart`
2. Right-click panel → Panel → Add New Items
3. Find "Clipboard History" and add it

## Configuration

Configure cliphist via the systemd service:
`~/.config/systemd/user/cliphist.service`

Default: monitors all clipboard MIME types (text, images, etc.)

## License

GPL-2.0-or-later
