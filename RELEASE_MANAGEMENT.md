# Release Management Policy

This document describes how versions are managed and released for the Game Audio Module, and how users should integrate the module into their projects.

## Versioning Strategy

This project follows [Semantic Versioning](https://semver.org/) (SemVer) using `MAJOR.MINOR.PATCH`:

- **MAJOR** (v2.0.0): Breaking API changes requiring code modifications (e.g., module rename from `audio_py` to `game_audio`)
- **MINOR** (v2.1.0): New backward-compatible features
- **PATCH** (v2.0.1): Backward-compatible bug fixes

**Current stable version**: v2.1.0

## Release Process

Releases are created by tagging commits on the `main` branch. The process works as follows:

1. **Feature development**: Work is done in feature branches that are merged to `main` when complete and tested
2. **Release preparation**: When all features/fixes for a release are merged, version numbers are updated
3. **Release creation**: A version tag is created and pushed (e.g., `v2.0.0`)

When a version tag is pushed, the release workflow handles:

1. **Automated testing**: Full test suite runs on Windows, Linux, and macOS
2. **Documentation**: Versioned docs are generated and archived
3. **Release creation**: GitHub Release is created with changelog from git commits

## Using Specific Versions

### CMake Integration (Recommended)

Projects should pin to a specific version tag in their `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(
    audio_module
    GIT_REPOSITORY https://github.com/hannaharmon/game-audio
    GIT_TAG v1.0.0  # Pin to specific version for stability
)
FetchContent_MakeAvailable(audio_module)
```

### Why Version Tags Are Recommended

Using version tags (e.g., `v1.0.0`) instead of the `main` branch provides:

1. **Stability**: Projects won't break when new features or breaking changes are added to `main`
2. **Predictability**: Projects use a known version with matching documentation
3. **Control**: Projects can upgrade to newer versions on their own timeline
4. **Reproducibility**: Builds are consistent - the same tag always produces the same code
5. **Safety**: Breaking changes in `main` won't affect projects until they explicitly upgrade

### What Happens When Using `main` Branch

Projects that use the `main` branch instead of version tags may experience:

- **Breaking changes**: Projects may break when breaking changes are merged to `main`
- **Documentation mismatch**: Documentation may not match the code being used
- **Inconsistent builds**: Builds may vary as `main` evolves over time
- **No control**: Projects have no control over when changes affect them

**Best practice**: Production projects should always pin to a specific version tag. The `main` branch should only be used for development or testing purposes.

### Version Selection Guidance

- **New projects**: Should use the latest version tag (check [GitHub Releases](https://github.com/hannaharmon/game-audio/releases))
- **Existing projects**: Should pin to their current version and upgrade when ready
- **Stability**: Patch versions (e.g., `v2.0.1`) contain only bug fixes
- **Breaking changes**: Major versions (e.g., `v2.0.0`) require code updates

## Versioned Documentation

Documentation is available for all released versions:

- **Main site**: https://hannaharmon.github.io/game-audio
  - Landing page with version dropdown switcher
  - Defaults to "Latest" (main branch)
- **Versioned docs**: Available at `/v1.0.0/`, `/v2.0.0/`, etc.
- **Direct links**: https://hannaharmon.github.io/game-audio/v1.0.0/

Older versions are provided for reference only. New projects should use the latest version.

## Support Policy

- **Latest version**: Active development and support
- **Previous versions**: Documentation available for reference only
- **No backports**: Bug fixes and features are only added to the latest version

Breaking changes between major versions are documented in release notes.

## CI/CD Workflow

### Automated Testing

Tests run automatically on:
- **Pull requests** targeting `main` branch
- **Pushes to `main`** branch
- **Manual trigger** via GitHub Actions UI

This ensures all code merged to `main` has been tested across Windows, Linux, and macOS.

### Documentation Updates

Documentation updates automatically on:
- **Every push to `main`** branch - The "latest" docs always reflect the current state of `main`, even if there's no release tag yet
- **Manual trigger** via GitHub Actions UI

Documentation does not update on pull requests or feature branch pushes. This ensures the public docs site only shows stable, merged code.

## Release Notes

Each release includes automatically generated changelogs from git commit history. Release notes are available on the [GitHub Releases page](https://github.com/hannaharmon/game-audio/releases).
