#!/bin/bash
# Signs a VST3 or Component bundle
# Usage: ./sign_plugin.sh <path_to_bundle>

BUNDLE_PATH="$1"

if [ -z "$BUNDLE_PATH" ]; then
  echo "Usage: $0 <path_to_bundle>"
  exit 1
fi

if [ ! -d "$BUNDLE_PATH" ]; then
  echo "Error: Bundle not found at $BUNDLE_PATH"
  exit 1
fi

echo "--- Signing $(basename "$BUNDLE_PATH") ---"
# Clear extended attributes (fixes "resource fork" errors)
xattr -cr "$BUNDLE_PATH"
# Sign with ad-hoc signature
codesign --force --deep --sign - "$BUNDLE_PATH"
echo "--- Signed: $BUNDLE_PATH ---"
