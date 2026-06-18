# Suggested follow-on agent customizations for rocSPARSE

Companion to [AGENTS.md](AGENTS.md). These are concrete next customizations to add when you want agents to be even more autonomous in this repo. Each entry includes the slash command to create it, what it should contain, and how an agent would use it in practice.

## 1. Prompt: scaffold a new sparse routine

**Create with:** `/create-prompt add-routine`

**Why:** Adding a routine in rocSPARSE is a six-file change with mandatory CMake + docs edits. A parameterized prompt eliminates the boilerplate and the "forgot to register in CMakeLists" failure mode.

**Suggested body (`.github/prompts/add-routine.prompt.md`):**

- Input: routine name (e.g. `csrxyz`), API category (`level1|level2|level3|extra|precond|conversion|reordering|generic|util`), and which precisions (default `s/d/c/z`).
- Steps the prompt instructs the agent to perform, using `csrmv` as the reference template:
  1. Create public header `library/include/internal/<category>/rocsparse_<name>.h` with `extern "C"` declarations for each precision.
  2. Include it from `library/include/rocsparse-functions.h`.
  3. Create `library/src/<category>/rocsparse_<name>.{cpp,hpp}` and `<name>_device.h` following the three-file pattern in [AGENTS.md](AGENTS.md#project-specific-conventions).
  4. Add the new sources to `library/src/CMakeLists.txt`.
  5. Create `clients/testings/testing_<name>.cpp`, `clients/tests/test_<name>.cpp`, `clients/tests/test_<name>.yaml` (include `rocsparse_common.yaml`, use the `alpha_beta_range_{quick,checkin,nightly}` anchors).
  6. Register the new test sources in `clients/testings/CMakeLists.txt` and `clients/tests/CMakeLists.txt`.
  7. Add an API entry to `docs/reference/<category>.rst`.
  8. Add the AMD MIT license header on every new file with the current year.
  9. Run `/opt/rocm/llvm/bin/clang-format -style=file -i` on all created files.

**How it's used in practice:** "/add-routine csrxyz level2 sdcz" produces a compilable, formatted skeleton ready for the actual kernel logic and YAML parameter expansion.

## 2. File instruction: client tests + YAML schema

**Create with:** `/create-instruction tests`

**Why:** The `clients/tests` + `clients/testings` split and the YAML parameter matrix are unique to this repo. Without scoped guidance, agents tend to put logic in the wrong file or forget the `rocsparse_common.yaml` include.

**Suggested file (`.github/instructions/tests.instructions.md`) with frontmatter:**

```yaml
---
applyTo: "clients/{tests,testings}/**"
description: "Conventions for rocSPARSE client gtest harness and YAML parameter matrices"
---
```

**Body should encode:**

- Host-side logic, bad-arg coverage, and reference-vs-device comparison live in `clients/testings/testing_<routine>.cpp`. The gtest file in `clients/tests/` is a thin `TEST_ROUTINE(<name>, <category>, ...)` wrapper.
- Every new `test_<routine>.yaml` must start with `include: rocsparse_common.yaml` and use the shared YAML anchors (`&alpha_beta_range_quick`, `&_checkin`, `&_nightly`) so cases get bucketed into the right CI tier.
- Both `testing_*.cpp` and `test_*.{cpp,yaml}` must be added to their respective `CMakeLists.txt` files.
- Test executables to run: `./clients/staging/rocsparse-test --gtest_filter='*<routine>*'` from the build dir.
- Matrices are downloaded by `cmake/ClientMatrices.cmake` on first configure into `${PROJECT_BINARY_DIR}/matrices`; reference that path, do not re-download.

## 3. Hook: enforce ROCm clang-format

**Create with:** `/create-hook clang-format`

**Why:** CI's static-analysis stage rejects diffs formatted by anything other than `/opt/rocm/llvm/bin/clang-format`. A `PostToolUse` hook fixes this deterministically instead of relying on the agent remembering.

**Suggested behavior (`.github/hooks/clang-format.json`):**

- Trigger: `PostToolUse` on file-edit tools (write, edit, multi-edit).
- Match: paths under `library/**` or `clients/**` ending in `.c`, `.cc`, `.cpp`, `.h`, `.hpp`, `.h.in`, `.hpp.in`, `.cpp.in`.
- Command: `/opt/rocm/llvm/bin/clang-format -style=file -i "$FILE"` (guard with `[ -x /opt/rocm/llvm/bin/clang-format ]` so the hook is a no-op on machines without ROCm installed).
- Do not format files under `build/`, `deps/external/`, or anything inside `.git/`.

**How it's used in practice:** Agents can edit freely; the hook normalizes formatting before commits, eliminating the most common CI failure on this repo.

## 4. (Optional) Skill: doc-build sanity check

**Create with:** `/create-skill build-docs`

**Why:** Doc build is part of CI and breaks easily when adding new `.rst` files without a `toctree` entry. A skill that runs the local Sphinx build and surfaces broken refs is cheap insurance for API-doc-touching changes.

**Suggested steps:**

1. `cd docs && pip3 install -r sphinx/requirements.txt` (only if not already installed).
2. `python3 -m sphinx -T -E -W -b html -d _build/doctrees -D language=en . _build/html` (note `-W` to fail on warnings, matching CI).
3. Grep the output for `WARNING`/`ERROR` and report the offending file+line.

---

## Iteration

After a few real coding sessions, run `/chronicle improve` — it inspects past sessions for friction patterns (commands that failed, files that were repeatedly searched, edits that had to be redone) and proposes concrete edits to [AGENTS.md](AGENTS.md) and any of the customizations above.
