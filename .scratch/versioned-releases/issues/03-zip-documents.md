# 03 readme.html and changelog.html in every zip

Status: ready-for-agent
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
