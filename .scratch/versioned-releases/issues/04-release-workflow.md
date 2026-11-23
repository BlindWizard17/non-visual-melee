# 04 The versioned release workflow

Status: ready-for-agent
Type: task
Blocked by: 03

Spec: `.scratch/versioned-releases/spec.md`, "The workflow". ADR-0011.

## What to do

- **Trigger**: pushed tags matching `a11y-v*`.
- **`MELEE_VERSION`** is set from the root `VERSION` file in every build job, for every trigger, so a tag build's app version is the base port's and never the tag's name.
- **The checks**, before any build. On any failure the run stops and says which check failed:
  - The tag is `a11y-v` plus `src/pc/a11y/VERSION`.
  - The first version section of `CHANGELOG.md` is that version.
  - The tagged commit is on `origin/master`.
- **The builds**: the Windows job alone, unlabelled (issue 02's build label). The Mac jobs are skipped on tags.
- **The notes**, in the spec's order:
  1. The header: "Plays online with" the fork and base versions, the requirements, and the link to `ACCESSIBILITY.md` at the tag.
  2. The version's changelog section without its heading.
  3. The link to `CHANGELOG.md` at the tag.
- **Where the logic lives**: the checks and the notes go in a small script, such as `tools/a11y/release_notes.py`, which runs the same locally. Its `--help` says what it checks and prints. Avoid a long inline shell step.
- **The release**:
  - It is created as a draft titled "Non-Visual Melee <version> beta", not a prerelease, with the Windows zip.
  - A run for a tag whose release already exists, draft or published, fails.
- **The header comment** of `a11y.yml` describes the tag build.
- **CLAUDE.md**'s CI paragraph says what a tag does.

## Done when

- The script, run locally against a `CHANGELOG.md` with a test version section, prints the notes and refuses each mismatch. Test the mismatches through its arguments, with no tag pushed.
- A dispatch on a topic branch still builds all three zips and leaves `ci-latest` and the releases alone.
- The tag path itself runs first for real in issue 07, as a draft, which no player sees.
- `python tools/check_style.py` passes.
