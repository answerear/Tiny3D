#!/usr/bin/env bash
# =====================================================
#  setup-ai-links.sh
#  Creates .cursor and .codebuddy symlinks
#  pointing to the shared .ai directory.
#  Run once after cloning the repository.
# =====================================================

set -e
cd "$(dirname "$0")"

if [ ! -d ".ai" ]; then
    echo "[ERROR] .ai directory not found. Make sure you are in the project root."
    exit 1
fi

# --- .cursor ---
if [ -e ".cursor" ]; then
    echo "[SKIP] .cursor already exists."
else
    ln -s .ai .cursor
    echo "[OK]   .cursor -> .ai"
fi

# --- .codebuddy ---
if [ -e ".codebuddy" ]; then
    echo "[SKIP] .codebuddy already exists."
else
    ln -s .ai .codebuddy
    echo "[OK]   .codebuddy -> .ai"
fi

echo ""
echo "Done. Both Cursor and CodeBuddy should now read rules from .ai/"
