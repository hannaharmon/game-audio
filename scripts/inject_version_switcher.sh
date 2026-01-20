#!/bin/bash
# Script to inject version switcher into all Doxygen-generated HTML pages

DOCS_DIR=$1
VERSIONED_DIR=$2

if [ -z "$DOCS_DIR" ] || [ -z "$VERSIONED_DIR" ]; then
    echo "Usage: inject_version_switcher.sh <docs_dir> <versioned_dir>"
    exit 1
fi

# Get list of available versions (only v*.*.* patterns like v1.0.0, exclude "versioned" and "latest" directories)
# Use find with pattern matching and grep to filter for semantic version pattern
VERSIONS=$(find "${VERSIONED_DIR}" -maxdepth 1 -type d -name "v*" | sed 's|.*/||' | grep -E '^v[0-9]+\.[0-9]+\.[0-9]+' | sort -V -r)

# Build version options HTML
VERSION_OPTIONS_HTML="<option value=\"latest\">Latest (main branch)</option>"
for v in $VERSIONS; do
    VERSION_OPTIONS_HTML="${VERSION_OPTIONS_HTML}
        <option value=\"${v}\">${v}</option>"
done

# Create version switcher HTML/JS using a temp file to avoid heredoc issues
TEMP_FILE=$(mktemp)
cat > "$TEMP_FILE" << 'EOFHERE'
<!-- Version Switcher -->
<div id="version-switcher-container" style="position: fixed; top: 10px; right: 10px; z-index: 10000; background: #2d2d2d; padding: 8px 12px; border: 1px solid #555; border-radius: 4px; box-shadow: 0 2px 8px rgba(0,0,0,0.3); font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;">
    <label for="version-switcher" style="display: block; font-size: 11px; margin-bottom: 4px; font-weight: 600; color: #bbb; text-transform: uppercase; letter-spacing: 0.5px;">Version:</label>
    <select id="version-switcher" onchange="switchDocVersion()" style="padding: 6px 8px; font-size: 13px; border: 1px solid #555; border-radius: 3px; cursor: pointer; background: #1e1e1e; color: #ddd; min-width: 160px; outline: none; transition: border-color 0.2s;">
EOFHERE

# Insert version options
echo "$VERSION_OPTIONS_HTML" >> "$TEMP_FILE"

# Append closing tags and script
cat >> "$TEMP_FILE" << 'EOFHERE'
    </select>
</div>
<style>
    #version-switcher:hover {
        border-color: #777 !important;
    }
    #version-switcher:focus {
        border-color: #4a9eff !important;
    }
    #version-switcher option {
        background: #1e1e1e;
        color: #ddd;
    }
</style>
<script>
    function switchDocVersion() {
        const select = document.getElementById('version-switcher');
        const version = select.value;
        
        // Get current path and extract the page path
        const currentPath = window.location.pathname;
        
        // Extract the page path (everything after /game-audio/latest/ or /game-audio/v*.*.*/)
        // For GitHub Pages: /game-audio/latest/index.html -> index.html
        //                  /game-audio/latest/classes.html -> classes.html
        let pagePath = currentPath;
        
        // Remove GitHub Pages base path and version prefix
        pagePath = pagePath.replace(/^.*\/game-audio\/(v[0-9.]+|latest)\//, '');
        
        // If we couldn't extract a path (shouldn't happen), default to index.html
        if (!pagePath || pagePath === '' || pagePath === '/') {
            pagePath = 'index.html';
        }
        
        // Construct the new URL
        // For "latest", go to /game-audio/latest/<page>
        // For versions, go to /game-audio/<version>/<page>
        const baseUrl = window.location.origin;
        let newUrl;
        
        if (version === 'latest') {
            newUrl = baseUrl + '/game-audio/latest/' + pagePath;
        } else {
            newUrl = baseUrl + '/game-audio/' + version + '/' + pagePath;
        }
        
        window.location.href = newUrl;
    }
    
    // Set current version in dropdown based on URL
    (function() {
        const path = window.location.pathname;
        const versionMatch = path.match(/\/game-audio\/(v[0-9.]+|latest)\//);
        if (versionMatch) {
            const currentVersion = versionMatch[1];
            const select = document.getElementById('version-switcher');
            if (select) {
                select.value = currentVersion;
            }
        } else {
            // If we're on a page without a version in the path, default to "latest"
            const select = document.getElementById('version-switcher');
            if (select) {
                select.value = 'latest';
            }
        }
    })();
</script>
EOFHERE

VERSION_SWITCHER_HTML=$(cat "$TEMP_FILE")
rm -f "$TEMP_FILE"

# Save switcher HTML to temp file for Python script
SWITCHER_TEMP=$(mktemp)
printf '%s\n' "$VERSION_SWITCHER_HTML" > "$SWITCHER_TEMP"

# Inject into all HTML files in docs directory using Python
python3 << PYTHON_SCRIPT
import os
import sys
import glob

docs_dir = "${DOCS_DIR}"
switcher_file = "${SWITCHER_TEMP}"

# Read switcher HTML
with open(switcher_file, 'r', encoding='utf-8') as f:
    switcher_html = f.read()

# Find all HTML files
html_files = []
for root, dirs, files in os.walk(docs_dir):
    for file in files:
        if file.endswith('.html'):
            html_files.append(os.path.join(root, file))

injected_count = 0
for html_file in html_files:
    try:
        with open(html_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Skip if already injected
        if 'version-switcher-container' in content:
            continue
        
        # Inject before </body>
        if '</body>' in content:
            content = content.replace('</body>', switcher_html + '\n</body>')
        else:
            # No </body> tag, append at end
            content = content + '\n' + switcher_html
        
        with open(html_file, 'w', encoding='utf-8') as f:
            f.write(content)
        
        injected_count += 1
    except Exception as e:
        print(f"Warning: Failed to inject into {html_file}: {e}", file=sys.stderr)

print(f"Version switcher injected into {injected_count} of {len(html_files)} HTML files")
PYTHON_SCRIPT

rm -f "$SWITCHER_TEMP"
