#!/bin/bash
# Script to inject version switcher into all Doxygen-generated HTML pages

DOCS_DIR=$1
VERSIONED_DIR=$2

if [ -z "$DOCS_DIR" ] || [ -z "$VERSIONED_DIR" ]; then
    echo "Usage: inject_version_switcher.sh <docs_dir> <versioned_dir>"
    exit 1
fi

# Get list of available versions
VERSIONS=$(find "${VERSIONED_DIR}" -maxdepth 1 -type d -name "v*" | sed 's|.*/||' | sort -V -r)

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
<div id="version-switcher-container" style="position: fixed; top: 10px; right: 10px; z-index: 10000; background: white; padding: 10px; border: 1px solid #ddd; border-radius: 4px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">
    <label for="version-switcher" style="display: block; font-size: 12px; margin-bottom: 5px; font-weight: 600;">Version:</label>
    <select id="version-switcher" onchange="switchDocVersion()" style="padding: 5px; font-size: 14px; border: 1px solid #ccc; border-radius: 4px; cursor: pointer;">
EOFHERE

# Insert version options
echo "$VERSION_OPTIONS_HTML" >> "$TEMP_FILE"

# Append closing tags and script
cat >> "$TEMP_FILE" << 'EOFHERE'
    </select>
</div>
<script>
    function switchDocVersion() {
        const select = document.getElementById('version-switcher');
        const version = select.value;
        
        // Determine current path structure
        const currentPath = window.location.pathname;
        let basePath = '';
        
        // If we're in a versioned directory (v1.0.0/ or latest/), go up two levels
        if (currentPath.match(/\/(v[0-9.]+|latest)\//)) {
            basePath = '../../';
        } else {
            // If we're at root, use relative paths
            basePath = './';
        }
        
        // Get current page path relative to version root
        let currentPage = currentPath;
        // Remove version prefix if present
        currentPage = currentPage.replace(/.*\/(v[0-9.]+|latest)\//, '');
        // Remove GitHub Pages base path if present
        currentPage = currentPage.replace(/.*\/game-audio\//, '');
        // Ensure we have a page name
        if (!currentPage || currentPage === '/') {
            currentPage = 'index.html';
        }
        
        if (version === 'latest') {
            window.location.href = basePath + 'latest/' + currentPage;
        } else {
            window.location.href = basePath + version + '/' + currentPage;
        }
    }
    
    // Set current version in dropdown based on URL
    (function() {
        const path = window.location.pathname;
        const versionMatch = path.match(/\/(v[0-9.]+|latest)\//);
        if (versionMatch) {
            const currentVersion = versionMatch[1];
            const select = document.getElementById('version-switcher');
            if (select) {
                select.value = currentVersion;
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
