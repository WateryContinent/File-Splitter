#!/bin/bash

# Name of your app
APP_NAME="File Splitter"
# Path to the directory where the script is located
SCRIPT_DIR=$(dirname "$(realpath "$0")")  # Use realpath for the absolute path

# Define the path to your executable
EXECUTABLE_PATH="$SCRIPT_DIR/File Splitter"  # Replace 'your_executable' with the actual executable name

# Create the directory structure for the .app bundle
APP_DIR="$SCRIPT_DIR/$APP_NAME.app"
FRAMEWORKS_DIR="$APP_DIR/Contents/Frameworks"
RESOURCES_DIR="$APP_DIR/Contents/Resources"
MACOS_DIR="$APP_DIR/Contents/MacOS"
ASSET_DIR="$SCRIPT_DIR/asset"

# Define the source directory for .dylib files
SOURCE_DIR="$SCRIPT_DIR/asset/libs"

# Create the app directory and subdirectories
mkdir -p "$FRAMEWORKS_DIR" "$RESOURCES_DIR" "$MACOS_DIR"

# Copy the app icon to the Resources folder
cp "$ASSET_DIR/app.icns" "$RESOURCES_DIR"

# Check if the source directory exists for .dylib files
if [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: Source directory for .dylib files ($SOURCE_DIR) does not exist."
    exit 1
fi

# Copy all .dylib files from the source directory to the Frameworks folder
if cp "$SOURCE_DIR"/*.dylib "$FRAMEWORKS_DIR" 2>/dev/null; then
    echo "Successfully copied .dylib files from $SOURCE_DIR to $FRAMEWORKS_DIR."
else
    echo "No .dylib files found in $SOURCE_DIR."
fi

# Check if the executable exists before trying to copy
if [[ ! -f "$EXECUTABLE_PATH" ]]; then
    echo "Error: Executable not found at $EXECUTABLE_PATH"
    exit 1
fi

# Copy the executable to the MacOS folder inside the .app bundle
cp "$EXECUTABLE_PATH" "$MACOS_DIR/$APP_NAME"

# Check if the copy was successful
if [[ $? -ne 0 ]]; then
    echo "Error: Failed to copy the executable to $MACOS_DIR"
    exit 1
fi

# Create Info.plist
cat << EOF > "$APP_DIR/Contents/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundleExecutable</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>com.watery.$APP_NAME</string>
    <key>CFBundleVersion</key>
    <string>1.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleIconFile</key>
    <string>app.icns</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
</dict>
</plist>
EOF

install_name_tool -add_rpath @executable_path/../Frameworks "$EXECUTABLE_PATH"
echo "Successfully created app bundle at: $APP_DIR"

