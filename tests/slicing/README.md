# Slicing Tests

## Overview

167 unique test cases, each run across 6 configuration combinations:
- `-pta`: `fi`, `fs`, `inv`
- `-cd-alg`: `ntscd`, `classic`

Total: **324 test runs** via CTest.

## How Tests Work

Each test:
1. **Sanity check** — compiles and runs the *unsliced* program to confirm expected output
2. **Compile** — compiles the C source to LLVM bitcode (`clang -emit-llvm`)
3. **Slice** — runs `llvm-slicer -c test_assert` on the bitcode
4. **Execute** — runs the sliced bitcode with `lli`
5. **Check output** — compares stdout to expected:
   - Default: must print `Assertion PASSED` (never `Assertion FAILED`)
   - Custom: match a `.output` file line-by-line

Tests are **correctness-only** — pass/fail. No timing or slice size measurement.

## Running Tests

```bash
# Run all tests (from the CMake build directory)
make check

# Run a single test with a specific config (from this directory)
./test-runner-debug.py <test-name> -cd-alg=ntscd -pta=fi

# Run a single test across all configs
./test-runner-debug.py <test-name>
```

## Adding a Test

1. Add a C source file to `sources/`
2. Register it in `tests.py`:
   ```python
   'my-test' : Test('my-test.c'),
   ```
3. Optionally add a `sources/my-test.output` file for expected multi-line output.

The `test_assert(expr)` macro is available in all tests (included automatically).
When `expr` is true it prints `Assertion PASSED`, when false `Assertion FAILED`.

## Measuring Slice Size

The `llvm-slicer` tool supports a `-statistics` flag that prints instruction counts
before and after slicing (to stderr). This is not used by the test runner but can be
invoked manually:

```bash
llvm-slicer -statistics -c test_assert -cd-alg=ntscd -pta=fi input.bc -o sliced.bc
```

Output line format:
```
Globals/Functions/Blocks/Instr.: <G> <F> <B> <I>
```
