# 03 readme.html and changelog.html in every zip

Status: resolved (2026-11-23)
Type: task
Blocked by: 01, 02

Spec: `.scratch/versioned-releases/spec.md`, "The zip's documents".

## What to do

- **`a11y.yml`**: pandoc makes `readme.html` from `ACCESSIBILITY.md` and `changelog.html` from `CHANGELOG.md`.
  - The command is `-f gfm -s`, page language English, titles "Non-Visual Melee" and "Non-Visual Melee changes".
  - Both files go into the Windows zip beside `melee.exe` and into each Mac zip beside `Melee.app`.
  - Install pandoc in each job, or make the files once and share them as an artifact, whichever keeps the file simpler.
  - A missing or empty HTML file fails the job.
- **The Mac jobs' "Check the zip"** lists the two files among the folder's contents.
- **`ACCESSIBILITY.md`**, Getting started: the zip holds `readme.html`, this document, and `changelog.html`, what changed in each release.
- **The `verify` skill**: the Mac zip's expected listing gains the two files.

## Done when

- A dispatch of `a11y.yml` on a topic branch passes, pushed with the maintainer's leave. The run's Windows and Mac zips each hold both files: download the artifacts and list them, never run them.
- `readme.html` opened from a scratch folder has `ACCESSIBILITY.md`'s headings as `h1` to `h3`, `lang="en"` and its title. Check with `grep` on the file, or in the built-in browser pane.
- Nested lists in `ACCESSIBILITY.md` come out nested in `readme.html`.

## Comments

### 2026-11-23, implementation (agent)

- **The pages** are made by `tools/a11y/make_documents.py OUT_DIR`, which every job calls in one line, so issue 04's tag build reuses it in its Windows job as it stands: `pandoc -f gfm -s --metadata pagetitle=<title> --metadata lang=en`. It exits 1, naming the file, when pandoc is not found, fails, or leaves a page missing or empty. Its test, `tools/a11y/test_make_documents.py` (ctest `make_documents`), fakes pandoc: the commands, and each refusal.
- **`pagetitle`, not the spec's `title`.** pandoc's template puts a `title` in the page as an h1 above the document's own, and both documents already open with the same words, so each page would start with two identical headings. `pagetitle` sets the browser's title alone. The script's comment says so.
- **pandoc is installed in each job** (apt on the Windows job's Ubuntu runner, brew on the Mac runners) rather than made once and shared as an artifact: no job waits on another. The run's versions: 3.1.3 on Ubuntu, 3.14 on arm64 and 3.10.2 on Intel Homebrew; the three pages agree in their headings, title and language.
- **`a11y.yml`**: the Windows job adds both pages to the zip beside `melee.exe` and lists them with `unzip -l`, which fails on one missing; each Mac job makes them into the zip's folder beside `Melee.app`, and "Check the zip" lists the folder and fails on either missing or empty. The header comment names the pages.
- **Docs**: `ACCESSIBILITY.md`'s Getting started says the folder holds `readme.html`, this guide, and `changelog.html`; the `verify` skill's Mac listing has both, and its unit tests line names `make_documents`. No line under Unreleased: the first beta's section is a summary, not a list.
- **The version file renamed** `src/pc/a11y/FORK_VERSION`, in its own commit: the first dispatch (run 45147713663) failed both Mac builds because libc++'s `#include <version>` opened `src/pc/a11y/VERSION` on the Mac's case-insensitive disk. Issue 02's comments have the details.
- **Spec open question 1** stays the maintainer's, in issue 07. Observed: `-s` adds no table of contents (pandoc's style has `#TOC` rules, but no contents block or `nav` is in the page); its style keeps a readable line width (`max-width: 36em`), left as it comes.
- **Verification**: CI and docs only, no game code, so no game run: `python tools/check_style.py` passes; after the rename `cmake --build build` passes and the unit tests pass 42 of 42; the configure refuses `v0.1` in `FORK_VERSION`. Dispatch run 45149342301 on `ci/zip-documents` (since deleted) passed all three jobs and left `ci-latest` alone (publish skipped). Its zips, downloaded and listed, never run: each holds `readme.html` and `changelog.html` beside `melee.exe` or `Melee.app`. Each `readme.html` has `lang="en"`, the title "Non-Visual Melee", and `ACCESSIBILITY.md`'s headings exactly, 1 h1, 6 h2 and 12 h3; nested lists come out nested ("Playable modes" holds its own list); no link is relative. Each `changelog.html` is titled "Non-Visual Melee changes", h1, Unreleased h2, New h3. The arm64 configure logged the label `dev build 399f322`.
