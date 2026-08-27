#!/bin/bash
# Maya Real-time Lighting Plugin - Drag & Drop Installer (Linux/macOS)
# This script can be dragged and dropped onto Maya to install the plugin

set -e

echo "========================================"
echo "MayaRealtimeLighting Plugin Installer"
echo "========================================"
echo ""

# Get system info
OS="$(uname -s)"
MEG_VERSION=${1:-2025}

if [ "$OS" = "Darwin" ]; then
    # macOS
    PLUGIN_DIR="$HOME/Library/Preferences/Autodesk/maya/$MEG_VERSION/plug-ins"
    PLUGIN_EXT=".bundle"
elif [ "$OS" = "Linux" ]; then
    # Linux
    PLUGIN_DIR="$HOME/maya/$MEG_VERSION/plug-ins"
    PLUGIN_EXT=".so"
else
    echo "Unsupported OS: $OS"
    exit 1
fi

echo "OS: $OS"
echo "Maya Version: $MEG_VERSION"
echo "Plugin Dir: $PLUGIN_DIR"
echo ""

# Create plugin directory
echo "Creating plugin directory..."
mkdir -p "$PLUGIN_DIR"
echo "✓ Done"
echo ""

# Find and copy plugin
echo "Looking for plugin file..."
PLUGIN_FILE=$(find . -name "MayaRealtimeLighting$PLUGIN_EXT" 2>/dev/null | head -1)

if [ -z "$PLUGIN_FILE" ]; then
    echo "✗ Plugin file not found!"
    echo "  Expected: MayaRealtimeLighting$PLUGIN_EXT"
    exit 1
fi

echo "✓ Found: $PLUGIN_FILE"
echo "Copying to plugin directory..."
cp "$PLUGIN_FILE" "$PLUGIN_DIR/"
echo "✓ Installed"
echo ""

echo "========================================"
echo "Installation Complete!"
echo "========================================"
echo ""
echo "Next steps:"
echo "  1. Restart Maya"
echo "  2. Go to Windows > Settings/Preferences > Plug-in Manager"
echo "  3. Search for 'MayaRealtimeLighting'"
echo "  4. Check 'Loaded' and 'Auto load'"
echo "  5. Click 'Apply'"
echo ""
