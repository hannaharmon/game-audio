#!/bin/bash
# Generate Doxygen documentation

echo "Generating Doxygen documentation..."
doxygen Doxyfile

echo ""
echo "Documentation generation complete."
echo "HTML documentation can be found in docs/html/"
echo ""