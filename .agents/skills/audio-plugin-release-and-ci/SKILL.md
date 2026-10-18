---
name: audio-plugin-release-and-ci
description: >-
  Standard workflow for building, packaging, CI/CD setup, and publishing direct 1-click
  downloads via GitHub Releases for audio plugins (VST3, AU, Standalone).
  Use whenever adding a new audio plugin, updating GitHub Actions workflows, or updating
  plugin download links in README.md.
---

# Audio Plugin Release & CI/CD Workflow

This skill defines the mandatory distribution standards for the `kijjaz/audio-plugins` repository.

## 🚨 Core Rules for Plugin Distribution

1. **NEVER use GitHub Actions Run Links as Primary Downloads:**
   - Bad: Linking to `https://github.com/.../actions/workflows/plugin.yml` as the download button.
   - Why: Users must click an individual build run, scroll down to find artifacts, and GitHub blocks users who are not logged in.
2. **ALWAYS use GitHub Releases for 1-Click Direct Downloads:**
   - Every plugin workflow must package pre-built binaries into `.zip` files and publish them to a GitHub Release (e.g. tag `v0.1.0` or `<plugin>-v0.1.0`).
   - Direct download links in `README.md` must follow the format:
     `https://github.com/kijjaz/audio-plugins/releases/download/<tag>/<Plugin>-<OS>.zip`
   - These direct links trigger an immediate file download in any web browser without requiring a GitHub account or login!

---

## 🛠️ GitHub Actions Workflow Template

Every plugin workflow in `.github/workflows/<plugin>.yml` must include:
1. `build-macos`: Universal binary (`arm64` + `x86_64`) building VST3, AU (`.component`), and Standalone App.
2. `build-windows`: Windows x64 VST3.
3. `release`: Triggered on push to `main` or tags, which downloads the built artifacts, zips them, and calls `softprops/action-gh-release@v2`.

### Standard Release Job Configuration

```yaml
  release:
    name: Publish Direct Download Release
    needs: [build-macos, build-windows]
    runs-on: ubuntu-latest
    if: github.event_name == 'push' && (github.ref == 'refs/heads/main' || startsWith(github.ref, 'refs/tags/'))
    permissions:
      contents: write
    steps:
      - name: Download Windows VST3
        uses: actions/download-artifact@v4
        with:
          name: <Plugin>-Windows-VST3
          path: windows_vst3/<Plugin>.vst3

      - name: Download macOS VST3
        uses: actions/download-artifact@v4
        with:
          name: <Plugin>-macOS-VST3
          path: macos_bundle/<Plugin>.vst3

      - name: Download macOS AU
        uses: actions/download-artifact@v4
        with:
          name: <Plugin>-macOS-AU
          path: macos_bundle/<Plugin>.component

      - name: Download macOS Standalone
        uses: actions/download-artifact@v4
        with:
          name: <Plugin>-macOS-Standalone
          path: macos_bundle/<Plugin>.app

      - name: Package Zip Bundles
        run: |
          mkdir -p release_pkgs
          # Zip macOS Universal Bundle
          cd macos_bundle
          zip -r ../release_pkgs/<Plugin>-macOS-Universal.zip <Plugin>.vst3 <Plugin>.component <Plugin>.app
          cd ..
          # Zip Windows VST3 Bundle
          cd windows_vst3
          zip -r ../release_pkgs/<Plugin>-Windows-x64.zip <Plugin>.vst3
          cd ..

      - name: Create or Update GitHub Release
        uses: softprops/action-gh-release@v2
        with:
          tag_name: <plugin>-v0.1.0
          name: "<Plugin> v0.1.0"
          files: |
            release_pkgs/<Plugin>-macOS-Universal.zip
            release_pkgs/<Plugin>-Windows-x64.zip
```

---

## 📋 README.md Formatting Standard

In the root `README.md`:

### 1. In the "Quick Download Hub" Table:
```markdown
| **<Plugin>** | Description (VST3, AU, Standalone) | [![Status](badge)](actions_url) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/<tag>/<Plugin>-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/<tag>/<Plugin>-Windows-x64.zip) | macOS, Windows |
```

### 2. In the Available Plugins Section:
```markdown
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download <Plugin>-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/<tag>/<Plugin>-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU, Standalone App)*
  - 🪟 **[Download <Plugin>-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/<tag>/<Plugin>-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/<plugin>.yml)**
```

---

## 🍎 Local macOS Signing and Validation

Before pushing any changes:
1. Strip quarantine and sign ad-hoc:
   ```bash
   xattr -cr ~/Library/Audio/Plug-Ins/VST3/<Plugin>.vst3 ~/Library/Audio/Plug-Ins/Components/<Plugin>.component
   codesign --force --deep -s - ~/Library/Audio/Plug-Ins/VST3/<Plugin>.vst3 ~/Library/Audio/Plug-Ins/Components/<Plugin>.component
   ```
2. Verify with `auval`:
   ```bash
   auval -v <type> <code4> <manu4>
   # Must exit with: AU VALIDATION SUCCEEDED
   ```
