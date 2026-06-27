#!/bin/bash
set -e

echo "=== Building Docker image ==="
docker build -t proyecto-paralelo:latest .

echo ""
echo "=== Build complete ==="
echo "Run with: docker run --rm proyecto-paralelo:latest"
echo ""
echo "Inside container:"
echo "  cd /app"
echo "  ./bin/sender   # Launch messages"
echo "  ./bin/receiver # Process messages"
