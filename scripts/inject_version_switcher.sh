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

# Inject into all HTML files in docs directory
find "${DOCS_DIR}" -name "*.html" -type f | while read -r html_file; do
    # Check if file already has version switcher
    if ! grep -q "version-switcher-container" "$html_file"; then
        # Inject before closing </body> tag
        # Use a more robust approach: escape special characters for sed
        ESCAPED_HTML=$(printf '%s\n' "$VERSION_SWITCHER_HTML" | sed 's/[[\.*^$()+?{|]/\\&/g')
        # Actually, better to use a different approach - append to a temp file and use it
        TEMP_INJECT=$(mktemp)
        printf '%s\n' "$VERSION_SWITCHER_HTML" > "$TEMP_INJECT"
        # Use perl for more reliable multi-line replacement
        perl -i -pe "BEGIN{undef \$/;} s|</body>|$(cat "$TEMP_INJECT")\n</body>|gs" "$html_file" 2>/dev/null || {
            # Fallback: simple sed (may have issues with special chars)
            sed -i "s|</body>|$(cat "$TEMP_INJECT")\n</body>|" "$html_file"
        }
        rm -f "$TEMP_INJECT"
    fi
done

echo "Version switcher injected into all HTML files in ${DOCS_DIR}"
