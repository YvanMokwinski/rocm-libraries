# rocSPARSE — Agent Guide

rocSPARSE is a HIP/C++ GPU sparse BLAS library targeting AMD ROCm. The public C API follows the "Hourglass" pattern: thin C89 surface, C++/HIP implementation behind opaque handles. This file gives AI agents the minimum project-specific knowledge to be productive; link out for the rest.

## Orientation

- High-level project description: [README.md](README.md)
- Contribution + code-style rules (authoritative): [.github/CONTRIBUTING.md](.github/CONTRIBUTING.md)
- Architecture, file layout, "what goes where" reference: [docs/conceptual/rocsparse-design.rst](docs/conceptual/rocsparse-design.rst)
- Release notes & recently changed APIs: [CHANGELOG.md](CHANGELOG.md)

This repo is normally consumed as a subdirectory of [`rocm-libraries`](https://github.com/ROCm/rocm-libraries) at `projects/rocsparse/`. It also builds standalone.

## Build & test

ROCm toolchain is required (`/opt/rocm/bin/amdclang++`). Don't substitute system `clang++`.

| Task | Command |
|------|---------|
| First-time build with deps + clients (tests/benchmarks/samples) | `./install.sh -dci` |
| Incremental build after source edits | `cmake --build build/release/release -j` (path created by `install.sh`) |
| Manual CMake config (clients on) | `mkdir -p build/release && cd build/release && CXX=/opt/rocm/bin/amdclang++ cmake -DBUILD_CLIENTS_TESTS=ON -DBUILD_CLIENTS_BENCHMARKS=ON ../..` |
| Python build wrapper (mirrors install.sh, used by CI) | `./rmake.py -c` |
| Run all gtest cases | `./clients/staging/rocsparse-test` (from the build dir) |
| Run a single routine's tests | `./clients/staging/rocsparse-test --gtest_filter='*csrmv*'` |
| Run a benchmark | `./clients/staging/rocsparse-bench -f csrmv --laplacian-dim 2000 -i 200` |
| Format one file (use ROCm's clang-format, NOT system) | `/opt/rocm/llvm/bin/clang-format -style=file -i <file>` |
| Format all tracked C/C++ | `git ls-files -z '*.cc' '*.cpp' '*.h' '*.hpp' '*.cl' '*.h.in' '*.hpp.in' '*.cpp.in' \| xargs -0 /opt/rocm/llvm/bin/clang-format -style=file -i` |
| Build docs locally | `cd docs && pip3 install -r sphinx/requirements.txt && python3 -m sphinx -T -E -b html -d _build/doctrees -D language=en . _build/html` |

Useful `install.sh` flags: `-i` install, `-d` install deps, `-c` clients, `-g` debug, `-a <gfx...>` GPU arch, `--clients-only` rebuild only clients against an installed library, `--memstat` enable allocation tracking, `--no-rocblas` skip rocBLAS dep. Run `./install.sh -h` for the full list.

CI runs static analysis (clang-format), doc build, and tests across multiple OS x `gfx*` targets — keep all three green.

## Repository layout (only the non-obvious parts)

- `library/include/` — public C API (`rocsparse.h` umbrella, `internal/` per-category headers).
- `library/src/{level1,level2,level3,extra,precond,conversion,reordering,generic,util,auxiliary,primitives,common,include}/` — implementations grouped by API category. `library/src/include/` holds shared infra (`handle.h`, `logging.h`, `definitions.h`, `common.h`).
- `clients/include/` + `clients/common/` — shared host-side test infrastructure (matrix generators, importers/exporters, checks, argument parsing).
- `clients/testings/` — per-routine `testing_<routine>.cpp` containing the actual host-side test logic. Reused by both `rocsparse-test` and `rocsparse-bench`.
- `clients/tests/` — gtest harness: `test_<routine>.cpp` (registers cases via `TEST_ROUTINE`) plus `test_<routine>.yaml` (parameter matrix). The harness reads YAML and dispatches into `testings/`.
- `clients/benchmarks/` — `rocsparse-bench` driver; also dispatches into `clients/testings/`.
- `clients/samples/` — standalone `example_<routine>.cpp` usage examples (built by default).
- `deps/convert.cpp` — `mtx2csr` matrix converter for test data.
- `cmake/ClientMatrices.cmake` — downloads test matrices into the build dir on first configure.
- `scripts/` — perf-bench plotting/regression helpers and `rocsparse-cppcheck.py`.
- `reproducibility/` — run-to-run reproducibility test harness.

## Project-specific conventions

**Routine source pattern.** Each subroutine lives in its category dir as three files:

- `rocsparse_<sub>.cpp` — `extern "C"` API wrappers for `s/d/c/z` (float/double/complex32/complex64) precisions; each returns `rocsparse_status` and forwards to the templated impl.
- `rocsparse_<sub>.hpp` — templated `<typename T>` implementation; uses the stream owned by `rocsparse_handle`.
- `<sub>_device.h` — `__device__` / `__global__` HIP kernels.

When adding a routine, support **at minimum** `float`, `double`, `rocsparse_float_complex`, `rocsparse_double_complex`. Mixed-precision and `_Float16`/`bfloat16` overloads are optional and only for routines where the CHANGELOG already shows precedent (`spmv`, `spmm`, `spgemm`).

**Testing a new/changed routine requires all three:**

1. `clients/testings/testing_<routine>.cpp` — host-side correctness + bad-arg coverage.
2. `clients/tests/test_<routine>.cpp` — gtest entry, typically a single `TEST_ROUTINE(<name>, <category>, ...)` macro invocation.
3. `clients/tests/test_<routine>.yaml` — parameter matrix; includes `rocsparse_common.yaml` and uses YAML anchors like `&alpha_beta_range_quick`, `&alpha_beta_range_checkin`, `&alpha_beta_range_nightly` to bucket cases by CI tier.

Add the new files to `clients/testings/CMakeLists.txt` and `clients/tests/CMakeLists.txt`. Existing routines (e.g. `csrmv`) are the canonical reference.

**Public-API discipline.** Public headers (`library/include/**`) must stay C89-compatible: only functions, opaque struct forward-decls, enums, typedefs, and pointers. No C++ types, no templates, no inline implementations. New API additions must be reflected in the corresponding `docs/reference/<category>.rst`.

**Status & errors.** API entry points return `rocsparse_status`. Convert HIP errors via the helper in `library/src/include/status.h`; never let a raw `hipError_t` leak out.

**Temporary device buffers.** When a routine needs scratch memory, expose a paired `*_buffer_size` query (caller allocates/frees, buffer is reusable). Do not allocate device memory inside the compute path. See the design doc for the rationale.

**Handles & streams.** Always use the stream from `rocsparse_handle` for kernel launches and async copies — don't capture the default stream.

**License header.** Every new C/C++/CMake/Python file needs the AMD MIT-style header (see `.github/CONTRIBUTING.md` for the exact block); update the copyright year on modified files (the pre-commit hook handles this).

**Formatting.** `WebKit`-based clang-format with 4-space indent and 100-col limit ([.clang-format](.clang-format)). Always format with the ROCm-bundled clang-format, not the system one — CI fails otherwise.

## Pitfalls observed in recent history

(From [CHANGELOG.md](CHANGELOG.md) "Resolved issues" — recurring failure modes worth watching for.)

- `__syncthreads` placement in kernels with early-exit or divergent branches. Several recent fixes were misuse of barriers in `bsrmm`, `csrmm`, `csx2dense`, `dense2csx`, `prune_dense2csr`, `csrcolor`, `csritilu0x`.
- Temporary-buffer sizing off-by-one — sort/check helpers needed `m+1` rather than `m` (e.g. `shift_offsets_kernel` in `csrsort`, `check_matrix_csr`).
- Complex conjugation — use `rocsparse_conj`, not `std::conj`, and only in the right paths (recent `bsric0` bug).
- `--rocsparse_ILP64` and `--memstat` build modes are not exercised by default; if you touch allocation entry points or index-type-templated code, build with `-DBUILD_ROCSPARSE_ILP64=ON` and `--memstat` to catch breakage.
- Don't shallow-copy structs that own device buffers (recent `csritsv` double-free); follow the existing deep-copy pattern in `rocsparse_copy_mat_info`.

## Where to ask docs to render

Public-API doc changes must show up in [docs/reference/](docs/reference/) (one file per API category, matching `library/src/` subdirs). Conceptual/architectural changes go in [docs/conceptual/](docs/conceptual/), how-tos in [docs/how-to/](docs/how-to/). The doc build is part of CI.
