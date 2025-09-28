<h3 align="center">Varn Programming Language</h3>
<p align="center">
<img src="https://img.shields.io/github/stars/funcieqDEV/Varn?style=for-the-badge" alt="GitHub stars">
<img src="https://img.shields.io/github/forks/funcieqDEV/Varn?style=for-the-badge" alt="GitHub forks">
<img src="https://img.shields.io/github/license/funcieqDEV/Varn?style=for-the-badge" alt="License">
<img src="https://img.shields.io/github/issues/funcieqDEV/Varn?style=for-the-badge" alt="Open issues">
</p>

> [!WARNING]
> Varn is in active development and not production-ready. Many features are unfinished.

---

## What is Varn? 🚀

**Varn** is a modern, low-level, statically typed programming language for system programming. It compiles to LLVM IR, enabling seamless interop with C/C++. Varn is inspired by C and Zig, but offers a more ergonomic, modular, and expressive syntax—without sacrificing control or performance.

Varn is **not** "C with new syntax". It is a new take on low-level programming: no garbage collection, no hidden abstractions, and no runtime overhead. You get full control over the hardware, with modern language features and zero-cost abstractions.

---

## Example Code
>There is no std for now, we only use C functions

**Hello, world:**

```varn
extern fun printf(fmt: str, ...) -> i32;

fun main() -> i32 {
	printf("Hello, world!\n");
	ret 0;
}
```


**String concatenation (no GC, manual memory):**

```varn
extern fun strlen(src: str) -> usize;
extern fun malloc(size: usize) -> str;
extern fun strcpy(dest: str, src: str) -> str;
extern fun strcat(dest: str, src: str) -> str;

fun concat(a: str, b: str) -> str {
	let lenA: usize = strlen(a);
	let lenB: usize = strlen(b);
	let totalLen: usize = lenA + lenB + 1;
	let result: str = malloc(totalLen);
	strcpy(result, a);
	strcat(result, b);
	ret result;
}
```

---

## Key Features

- **LLVM backend**: Easy C/C++ interop, native codegen, and toolchain support.
- **No garbage collection**: You manage memory—no hidden costs, no runtime.
- **Zero-cost abstractions**: Modern syntax, pattern matching, enums, and more—without runtime penalty.
- **Modular by default**: No `#include`, no header hell. Modules are first-class.
- **Powerful standard library**: Lightweight but rich, written from scratch for Varn. (WIP)
- **Inspired by C and Zig**: Familiar, but more ergonomic and expressive.
- **No forced abstractions**: You decide how close to the hardware you want to be.

---

## How to Build

```bash
git clone https://github.com/funcieqDEV/Varn
cd Varn
make
```

> ⚠️ Varn is under heavy development. The build may be unstable.

---


## License

Varn is licensed under the Apache License 2.0. See [LICENSE](LICENSE).
