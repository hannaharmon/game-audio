#!/bin/bash
# Script to build versioned documentation structure for GitHub Pages
# This creates a versioned docs structure with a dropdown switcher

set -e

VERSION=$1
DOCS_DIR="docs/html"
VERSIONED_DIR="docs/versioned"
LATEST_DIR="${VERSIONED_DIR}/latest"

# Create versioned directory structure
mkdir -p "${VERSIONED_DIR}"

if [ -n "$VERSION" ]; then
    # For a specific version (from release workflow)
    VERSION_DIR="${VERSIONED_DIR}/v${VERSION}"
    echo "Building docs for version v${VERSION}"
    
    # Copy docs to versioned directory
    rm -rf "${VERSION_DIR}"
    cp -r "${DOCS_DIR}" "${VERSION_DIR}"
    
    # Inject version switcher into all docs pages
    chmod +x scripts/inject_version_switcher.sh
    ./scripts/inject_version_switcher.sh "${VERSION_DIR}" "${VERSIONED_DIR}"
    
    echo "Documentation for v${VERSION} saved to ${VERSION_DIR}"
else
    # For latest (from main branch)
    echo "Building latest documentation"
    
    # Copy to latest directory
    rm -rf "${LATEST_DIR}"
    cp -r "${DOCS_DIR}" "${LATEST_DIR}"
    
    # Inject version switcher into all docs pages
    chmod +x scripts/inject_version_switcher.sh
    ./scripts/inject_version_switcher.sh "${LATEST_DIR}" "${VERSIONED_DIR}"
    
    # Create index page with version switcher
    cat > "${VERSIONED_DIR}/index.html" << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Game Audio Module - Documentation</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: #f5f5f5;
        }
        .container {
            background: white;
            padding: 40px;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        h1 {
            margin-top: 0;
            color: #333;
        }
        .version-selector {
            margin: 30px 0;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 6px;
            border-left: 4px solid #0366d6;
        }
        label {
            display: block;
            font-weight: 600;
            margin-bottom: 10px;
            color: #24292e;
        }
        select {
            width: 100%;
            padding: 10px;
            font-size: 16px;
            border: 1px solid #d1d5db;
            border-radius: 6px;
            background: white;
            cursor: pointer;
        }
        select:hover {
            border-color: #0366d6;
        }
        .info {
            margin-top: 20px;
            padding: 15px;
            background: #fff3cd;
            border-radius: 6px;
            border-left: 4px solid #ffc107;
            color: #856404;
        }
        .info strong {
            display: block;
            margin-bottom: 5px;
        }
        .redirect-notice {
            margin-top: 20px;
            font-style: italic;
            color: #6a737d;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Game Audio Module Documentation</h1>
        
        <div class="version-selector">
            <label for="version-select">Select Documentation Version:</label>
            <select id="version-select" onchange="switchVersion()">
                <option value="latest">Latest (main branch)</option>
                <!-- Versions will be populated by script -->
            </select>
        </div>
        
        <div class="info">
            <strong>Note:</strong>
            Select a version from the dropdown above to view its documentation. The page will not auto-redirect.
            Older versions are provided for reference only. We recommend upgrading to the latest version for new projects.
        </div>
    </div>
    
    <script>
        // Get list of available versions
        const versions = [];
        const latestOption = { value: 'latest', text: 'Latest (main branch)' };
        versions.push(latestOption);
        
        // This will be populated by the build script with actual versions
        
        function switchVersion() {
            const select = document.getElementById('version-select');
            const version = select.value;
            if (version === 'latest') {
                window.location.href = './latest/index.html';
            } else {
                window.location.href = `./${version}/index.html`;
            }
        }
        
        // Populate dropdown with available versions
        // Versions will be injected by build script
    </script>
</body>
</html>
EOF
    
    # Generate list of available versions
    VERSIONS=$(find "${VERSIONED_DIR}" -maxdepth 1 -type d -name "v*" | sed 's|.*/||' | sort -V -r)
    
    # Update index.html with version list
    if [ -n "$VERSIONS" ]; then
        # Create JavaScript array of versions
        VERSION_JS=""
        for v in $VERSIONS; do
            VERSION_JS="${VERSION_JS}                versions.push({ value: '${v}', text: '${v}' });\n"
        done
        
        # Insert versions into script
        sed -i "/\/\/ This will be populated/i${VERSION_JS}" "${VERSIONED_DIR}/index.html"
    fi
    
    echo "Latest documentation saved to ${LATEST_DIR}"
    echo "Index page created at ${VERSIONED_DIR}/index.html"
fi
