# Copilot instructions for this repository

## Build, test, and lint

This repository does not currently define any build, test, or lint commands.

- No `CMakeLists.txt`, `Makefile`, Meson, Bazel, `package.json`, or Python project metadata is present.
- No test framework or single-test invocation is configured in the current repo state.

## High-level architecture

The repository is currently a minimal asset-only project:

- `art/calc.png` is the only tracked project file in the working tree snapshot that was inspected.
- There is no application source tree, library code, executable entrypoint, or documented build pipeline yet.
- There are no repository docs such as `README.md` or `CONTRIBUTING.md`, and no alternative AI instruction files such as `CLAUDE.md`, `AGENTS.md`, or Cursor/Cline/Windsurf rules.

## Key conventions

- Keep repository guidance grounded in the current tree: future Copilot sessions should not assume a C++ build system or calculator implementation exists unless new files are added.
- Treat `art/` as the current home for project assets; today it contains the calculator image asset `art/calc.png`.
- If source code, tests, or tooling are added later, update this file with the concrete commands and architecture rather than relying on assumptions from the current minimal layout.
