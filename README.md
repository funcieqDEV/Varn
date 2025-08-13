<h3 align="center">Varn Programming Language</h3>
<p align="center">
<img src="https://img.shields.io/github/stars/funcieqDEV/Varn?style=for-the-badge" alt="GitHub stars">
<img src="https://img.shields.io/github/forks/funcieqDEV/Varn?style=for-the-badge" alt="GitHub forks">
<img src="https://img.shields.io/github/license/funcieqDEV/Varn?style=for-the-badge" alt="License">
<img src="https://img.shields.io/github/issues/funcieqDEV/Varn?style=for-the-badge" alt="Open issues">
</p>

> [!WARNING]
> Varn is in active development and is not production-ready. Many of the features and details described here are still unfinished.

---

## What is Varn? 🚀🛠️🛡️

**Varn** is a modern, statically typed programming language that focuses on **simplicity** and **safety**. It allows you to write low-level code like in C while maintaining a modern syntax similar to Rust. ✨📦⚡

---

## Why Varn? 🤔💡🛡️

You might ask: why use Varn at all? If I need low-level programming, I can use C, C++, or Zig. If I want safety, I can use Rust.
However, each has its limitations:

* C offers no memory protection, which can lead to dangerous errors.
* Rust has excellent safety but is verbose and enforces strict rules that can slow development.

**Varn** addresses these issues. You can write code freely, while **GMM** (Guardian Memory Model), a static analyzer, ensures memory safety. It detects common memory errors like use-after-free or dangling pointers **without runtime overhead**, giving you safe and efficient code.

---

## What is GMM? 🛡️📊✨

**GMM – Guardian Memory Model** functions similarly to Rust’s borrow checker. Every object has a "guardian" that monitors its usage.
Instead of imposing rigid rules, GMM politely informs you about potential memory errors.
If you intentionally use advanced features like FFI, you can instruct GMM to ignore certain objects. This balance provides safety while keeping development flexible.

---

## Goals 🎯📝✨

* Enable easy development of **system-level applications** that are both reliable and secure.
* Maintain a **high-quality developer experience** (UX) while handling low-level details.
* Reduce verbosity compared to Rust while retaining memory safety and compile-time guarantees.
* Encourage modular, maintainable code structures for larger projects.
* Provide expressive constructs for enums, structs, pattern matching, and inline lambdas.

---

## Build ⚙️🛠️📦

```bash
git clone https://github.com/funcieqDEV/Varn
cd Varn
make
```

> ⚠️ Warning: Varn is still in development. The build may be unstable.

---

## Contributing 🤝✨📘

We welcome all PRs! Each PR should:

* Include a clear description of what it adds.
* Explain why the change is needed.
* Include proper documentation for the added code.

---

## License 📜⚖️✨

The **Varn** project is licensed under the **Apache License 2.0**.
You may freely use, modify, and distribute the source code, provided that all copyright and license notices are retained.
The license also grants rights to any patents associated with the project.
See the **[LICENSE](https://github.com/funcieqDEV/Varn/blob/main/LICENSE)** file for full terms.
