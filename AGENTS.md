# Repository Guidelines

## Project Structure & Module Organization

- `src/`: application entrypoints (e.g. `standard.cpp`, `sentry.cpp`)
- `tasks/`: algorithm modules (auto-aim, planners, perception)
- `io/`: hardware + transport (camera/USB, `dm02` gimbal, optional ROS2 bridge)
- `tools/`: shared utilities (logging, debug/plotter, timing)
- `tests/`: integration-style test binaries (`*_test.cpp`)
- `configs/`, `assets/`, `data/`: runtime configs, calibration/assets, logs/recordings
- `3rdparty/`: vendored deps; `generated/`: codegen outputs; `_ref/`: reference snapshots (not built)

## Build, Test, and Development Commands

This repo is Docker-first (builds are executed inside the dev image).

- `make build-dev`: build the development image
- `make spv-build`: configure + build in container (uses `SPV_BUILD_DIR`, `SPV_BUILD_TYPE`, `SPV_CMAKE_ARGS`)
- `make spv-rebuild`: clean rebuild (removes `$(SPV_BUILD_DIR)` first)
- `make spv-run SPV_RUN_TARGET=auto_aim_test`: run a built binary from `$(SPV_BUILD_DIR)`
- `make spv-ros-build`: build `ros2_ws` packages (default `sp_msgs`)
- `make spv-build-with-ros`: build ROS workspace first, then build `sp_vision`

Device note: set `GIMBAL_DEV_HOST=/dev/ttyUSB0` (or your udev alias) when using the serial gimbal.

## Coding Style & Naming Conventions

- Format C++ with `.clang-format` (Google-based, 2-space indent, `ColumnLimit: 100`).
- Prefer `snake_case` for files/functions, `CamelCase` for types, `kConstant` for constants.
- Keep DM02 protocol/types under `io/dm02/` to avoid “scattered headers”.

## Testing Guidelines

- Tests are CMake executables sourced from `tests/` (not a `ctest` suite).
- Name new tests `tests/<name>_test.cpp`, add an `add_executable(<name>_test ...)` entry, and run with:
  `make spv-run SPV_RUN_TARGET=<name>_test`

## Commit & Pull Request Guidelines

- Commit messages follow `type: summary` (existing history uses `refactor: ...`; also use `feat:`, `fix:`, `docs:`, etc.).
- PRs must include: what changed, how it was verified (e.g. `make spv-build` + a `spv-run` target), and a note for any protocol-breaking change.

## Protocol Sync (Host <-> Firmware)

If you modify `io/dm02/protocol`, update the mirrored firmware-side protocol under
`_ref/printk/H7_new/DM-balanceV1/User/Communication` so both ends stay in lockstep.
