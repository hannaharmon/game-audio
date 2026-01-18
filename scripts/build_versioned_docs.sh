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
    
    # Create simple redirect index page that goes straight to latest docs
    cat > "${VERSIONED_DIR}/index.html" << 'EOF'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="0; url=./latest/index.html">
    <title>Game Audio Module - Documentation</title>
    <script>
        // Immediate redirect
        window.location.href = './latest/index.html';
    </script>
</head>
<body>
    <p>Redirecting to <a href="./latest/index.html">latest documentation</a>...</p>
</body>
</html>
EOF
    
    echo "Index redirect page created at ${VERSIONED_DIR}/index.html"
    
    echo "Latest documentation saved to ${LATEST_DIR}"
    echo "Index page created at ${VERSIONED_DIR}/index.html"
fi
