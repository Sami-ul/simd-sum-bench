# simd-sum-bench

Sums 1,000,000 floats two ways and times both: a plain loop, and a hand-written SIMD loop built with [xsimd](https://github.com/xtensor-stack/xsimd).

| Version | Best | Average | Speedup |
|---|---|---|---|
| Plain loop (`src/regular.cpp`) | ~490 us | ~510 us | 1x |
| SIMD, 8 accumulators (`src/simd.cpp`) | ~34 us | ~38 us | ~14x |

Measured on an Apple M5 Pro in High Power mode, with Apple clang 21 at `-O3`. In Automatic mode most runs took about 735 us and 50 us, so the speedup was the same.

## Run it

```sh
brew install xsimd
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build && ./build/main
```

`src/main.cpp` checks that both versions agree, then prints the best and the average time of 500 runs for each.

## Why several accumulators

With one accumulator, every add must wait for the add before it. The CPU can run several adds at once, but one chain of adds gives it only one to work on.

Eight accumulators are eight independent chains, so the adds overlap. They are combined once, after the loop.

Why eight: [measurements of the M5](https://github.com/useless-husband/m5-uarch) report 4 float/SIMD units per performance core and about 2 cycles per float add. That is 4 × 2 = 8 adds in flight at once.

The best count depends on the CPU. Change `chains` in `src/simd.cpp` to try another.

## Why `-O3` and not `-O0`

| Build | Plain loop (best / average) | SIMD (best / average) |
|---|---|---|
| `-O3` | ~490 / ~510 us | ~34 / ~38 us |
| `-O0` | ~1500 / ~2100 us | ~3300 / ~3700 us |

At `-O0` the SIMD version is the slower one. It still uses SIMD instructions, but each xsimd operation is a real function call and every batch is copied through memory. That overhead costs more than the SIMD saves.

At `-O3` the compiler inlines those calls, and each one becomes one or two instructions. Always benchmark the Release build.

To reproduce the `-O0` row:

```sh
cmake -B build-O0 -DCMAKE_BUILD_TYPE=Debug
cmake --build build-O0 && ./build-O0/main
```

## Why float and not int

Integer addition gives the same result in any order. The compiler is free to reorder it, so at `-O3` it turns the plain `int` loop into SIMD with several accumulators by itself. Hand-written SIMD gains little there.

Float addition rounds, so `(a + b) + c` can differ from `a + (b + c)`. The compiler must keep the order you wrote, which leaves the plain loop as one slow chain. Writing the SIMD by hand is how you say that a different order is acceptable.

The two float sums can therefore differ in the last digits. The check in `main.cpp` allows for that.
