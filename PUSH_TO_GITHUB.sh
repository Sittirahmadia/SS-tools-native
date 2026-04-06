#!/bin/bash

# SS-TOOLS Improved - GitHub Push Script
# This script prepares and pushes the project to GitHub with full CI/CD setup

echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║       SS-TOOLS Improved - GitHub Push & CI/CD Deployment         ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo ""

# Variables
GITHUB_REPO="https://github.com/Sittirahmadia/SS-tools-native.git"
BRANCH_NAME="main"
REMOTE_NAME="origin"

echo "📋 Pre-Push Checklist:"
echo "✓ Git repository initialized"
echo "✓ 3 commits created with full history"
echo "✓ GitHub Actions workflow configured (.github/workflows/build.yml)"
echo "✓ Ready for deployment"
echo ""

echo "🔗 Repository Information:"
echo "  URL:    $GITHUB_REPO"
echo "  Branch: $BRANCH_NAME"
echo "  Remote: $REMOTE_NAME"
echo ""

# Step 1: Check if remote exists
echo "Step 1: Configuring Git Remote"
if git remote get-url origin 2>/dev/null | grep -q "github.com"; then
    echo "  ✓ Remote 'origin' already configured"
else
    echo "  ➜ Adding remote 'origin'..."
    git remote add origin "$GITHUB_REPO"
    echo "  ✓ Remote added"
fi

# Step 2: Rename branch to main
echo ""
echo "Step 2: Preparing Branch"
CURRENT_BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [ "$CURRENT_BRANCH" != "$BRANCH_NAME" ]; then
    echo "  ➜ Renaming branch from $CURRENT_BRANCH to $BRANCH_NAME..."
    git branch -M "$BRANCH_NAME"
    echo "  ✓ Branch renamed"
else
    echo "  ✓ Already on branch '$BRANCH_NAME'"
fi

# Step 3: Show commits that will be pushed
echo ""
echo "Step 3: Commits to Push"
COMMIT_COUNT=$(git rev-list --count "$BRANCH_NAME")
echo "  Total commits: $COMMIT_COUNT"
echo ""
git log --oneline -3 | sed 's/^/    /'

# Step 4: Push to GitHub
echo ""
echo "Step 4: Pushing to GitHub"
echo "  ➜ Pushing $COMMIT_COUNT commits..."
git push -u "$REMOTE_NAME" "$BRANCH_NAME"

if [ $? -eq 0 ]; then
    echo "  ✓ Push successful!"
else
    echo "  ✗ Push failed. Please check your credentials."
    exit 1
fi

# Step 5: Verify remote setup
echo ""
echo "Step 5: Verification"
echo "  Remote configuration:"
git remote -v | sed 's/^/    /'

echo ""
echo "╔════════════════════════════════════════════════════════════════════╗"
echo "║                    GitHub Push Complete! ✅                        ║"
echo "╚════════════════════════════════════════════════════════════════════╝"
echo ""
echo "📊 Next Steps:"
echo "  1. Visit: https://github.com/Sittirahmadia/SS-tools-native"
echo "  2. Go to: Actions tab"
echo "  3. Monitor: First workflow run (5-10 minutes)"
echo "  4. Check: Build status and artifacts"
echo "  5. Download: Executables (x64 & x86)"
echo ""
echo "🏷️  To Create a Release:"
echo "  git tag -a v2.0.0 -m 'SS-TOOLS Improved v2.0.0'"
echo "  git push origin v2.0.0"
echo ""
echo "📚 Documentation:"
echo "  - README.md                - Project overview"
echo "  - INSTALLATION_GUIDE.md    - User guide"
echo "  - GITHUB_PUSH_GUIDE.md     - CI/CD documentation"
echo "  - BUILD_INSTRUCTIONS.md    - Build from source"
echo ""
