# Versioned releases

Status: agreed 2026-11-23 (design conversation with the maintainer)

The maintainer agreed:

- Non-Visual Melee gets version numbers of its own, apart from the base port's, and each release says which base port release it plays online with.
- Work happens on a `dev` branch, which `ci-latest` builds. `master` matches the latest release.
- Versioned releases ship the Windows zip alone until a Mac build is play-tested.
- Each zip holds `readme.html` and `changelog.html`.
- A `CHANGELOG.md` keeps the full history. Each release's notes hold that version's changes and link to the full changelog and to `ACCESSIBILITY.md`.
- The launcher's update check follows the fork's releases.
- A light release skill does the release, with no verify step: the maintainer runs verify before a release when they judge it worth it.

The decision and its rejected alternatives are ADR-0011.

## Goal

The maintainer can cut a first public beta, and every later release, with one skill run. A player finds the newest release by one link that never changes, and reads in the zip what the game does and what changed. A fork player's launcher tells them when a newer Non-Visual Melee is out, never a base port release.

## Scope

In:

- **The `dev` branch** and the move of everyday work, `ci-latest` and base merges onto it.
- **The fork version**: its file, its tag, its place in the build, the startup log and the launcher's shown version.
- **`CHANGELOG.md`** and the docs rules that keep it in step.
- **`readme.html` and `changelog.html`** in every zip CI makes, `ci-latest`'s Mac zips included.
- **The versioned release workflow** in `.github/workflows/a11y.yml`.
- **The launcher's update check** following fork releases.
- **The release skill.**
- **The first release**, 0.1.0.

Out:

- **Mac zips in versioned releases.** They come with the Mac play test (`.scratch/macos-port/issues/08-play-test-mac.md`). Adding them is a change to the tag build's job list and to this spec's "Windows only" rules, nothing else.
- **Downloading and installing an update from the launcher.** On Windows the base port opens the release page in the browser, and the fork keeps that.
- **Signing the Windows binaries or notarizing the Mac app.**
- **`melee.exe --version`**, which keeps printing the base port's version.

## What there is today

From reading the files on 2026-11-23.

### Versions

- The root `VERSION` file (`v0.4.2`) is the app version. The root `CMakeLists.txt` reads it into `MELEE_APP_VERSION` for `src/pc/version.cpp` (`pc::get_app_version()`, `pc_app_version()`).
- An environment variable `MELEE_VERSION` overrides it. On a CI run for a tag, `GITHUB_REF_NAME` overrides it when `MELEE_VERSION` is unset. The value must look like `v?N.N.N(-suffix)`, or the configure stops.
- Two copies pair online only when their app version strings match (CLAUDE.md, "Online compatibility").
- The local repository holds the base port's tags, `v0.1.0-beta` to `v0.4.2`, fetched from `upstream`.

### CI

- `.github/workflows/a11y.yml` builds the Windows zip and the two Mac zips on every push to `master`, and on a dispatch on any branch. Only a push to `master` publishes them, as the `ci-latest` prerelease, which is deleted and created again each time.
- The publish job needs the Windows job to pass and takes whichever Mac zips were built.
- The base port's `build.yml` (disabled in the fork) publishes a release on a `v*` tag, with notes from `.github/RELEASE_NOTES.md`, a base port file.

### The update check

`src/pc/updater.cpp` does the check, and `src/pc/launcher.cpp` drives it and shows its results.

- `check_for_updates_async` reads `api.github.com/repos/999sian/melee-pc/releases` and picks the newest release whose tag parses as a version (`SemVer::parse`). It compares that with the app version (`is_update_available`).
- `select_best_asset` looks for `Melee-<platform>-<arch>.zip` on Windows and the Mac.
- `open_release_in_browser` opens `github.com/999sian/melee-pc/releases`. On Windows an update is always installed from the browser.
- The messages name "Melee-PC" and the tag as written ("Update available: v0.4.3").
- The launcher shows the app version as `app-version` and `check-status` on Settings and as `menu-version` and `port-update-status` in the port menu. The fork's launcher reader already says the update card ("Update available: " and its `update-tag`).
- `drive.py --launcher` writes the update check off into the run's `launcher.cfg`.

### Docs

- `ACCESSIBILITY.md` links to `releases/latest` and says online play needs "the same version" as the other player.
- Its links are all full URLs today.

## Decisions

### Versions

- **The fork version** is `MAJOR.MINOR.PATCH`, `0.x` while in beta.
  - It lives in `src/pc/a11y/VERSION`, which holds the version of the latest release. It holds `0.0.0` until the first release.
  - `a11y.cmake` reads it into a define for fork code, as the root `CMakeLists.txt` does for the app version.
  - A minor bump is for new features and a patch bump for fixes alone. The release skill asks the maintainer which.
- **The build label** tells builds of the same version apart.
  - A tag build has none.
  - A CI build of anything else is labelled "dev build" with the commit's short hash.
  - A local build is labelled "local build".
  - CI passes the label in through the environment. It appears where the version is shown or logged, never where versions are compared.
- **The tag** is `a11y-v<version>`.
- **The release** is titled "Non-Visual Melee <version> beta" while the version is `0.x`.
  - It is published as a full release, not a prerelease, so `releases/latest` (and the link in `ACCESSIBILITY.md`) always reaches it.
  - `ci-latest` stays a prerelease and never becomes "latest".
- **The app version is never touched.** Every CI build job sets `MELEE_VERSION` from the root `VERSION` file, so a tag build cannot take the tag's name.
- **The startup log** gets one line naming both versions, such as "Non-Visual Melee 0.1.0 (dev build 1a2b3c4), Melee-PC v0.4.2". A `melee-pc.log` a player sends back then names the build.

### Branches

- **`dev`** holds everyday work. Topic branches start from it and merge back into it, and base merges go into it.
- **`master`** moves only at a release, by `git merge --ff-only dev`, so it is exactly what was released. It stays GitHub's default branch.
- **A hotfix** is committed on `master` or on a short branch from it, released as a patch, and then `master` is merged into `dev`, so the next fast-forward still works.
- **Online pairing.** A base merge that moves `VERSION` leaves the latest release paired with the old base release. The release skill's report says when `dev`'s `VERSION` differs from `master`'s, as a reminder to release soon.

### CHANGELOG.md

- **Where**: `CHANGELOG.md` at the repo root. The base port has none, so base merges never conflict there.
- **Shape**:
  - An `## Unreleased` section at the top, then one `## <version> (<date>)` section per release, newest first.
  - Each release section starts with "Plays online with Melee-PC <base version>.", written at release time from `VERSION`.
  - Within a section, `### New`, `### Changed` and `### Fixed`, each present only when it has entries.
  - The changelog is written for players, about what they meet, in plain words, a line or two per change.
- **The first release's section** is a short summary of what is accessible in the first beta, pointing to `ACCESSIBILITY.md`, not a list of every change so far.
- **Kept in step**: a change a player meets adds its line under Unreleased in the same change, as it keeps `ACCESSIBILITY.md` in step. Internal changes add nothing. A base merge adds a line when the base port's changes matter to players, and always when it moves `VERSION`.
- **Links**: `ACCESSIBILITY.md` and `CHANGELOG.md` link only by full URL, since their HTML copies are read from an unzipped folder. The Docs section of CLAUDE.md says so.

### The zip's documents

- **`readme.html`** is made from `ACCESSIBILITY.md` and **`changelog.html`** from `CHANGELOG.md`. Both sit beside `melee.exe`, and beside `Melee.app` in the Mac zips.
- **Made by pandoc** with GitHub-flavoured Markdown as input and a standalone page as output, with the page language English and the titles "Non-Visual Melee" and "Non-Visual Melee changes": `pandoc -f gfm -s --metadata title=... --metadata lang=en`.
  - Headings, lists and links come through as GitHub shows them.
  - A few lines of style for a readable line width are optional and must leave the headings alone.
- **Every zip CI makes gets them**: versioned releases and `ci-latest`, Windows and Mac, each from the commit it builds.
- **The install**: pandoc comes from the runner's package manager (`apt` on the Windows job's Ubuntu runner, `brew` on the Mac runners). Alternatively one job makes the two files and the others take them as an artifact. The implementer picks whichever keeps `a11y.yml` simpler.

### The workflow

All in `.github/workflows/a11y.yml`, with its header comment updated to match.

- **Triggers**:
  - A push to `dev` builds and publishes `ci-latest`, whose notes say "The newest build of dev".
  - A pushed tag matching `a11y-v*` builds a versioned release.
  - A push to `master` builds nothing, since its tag builds the release.
  - A dispatch on any branch builds and keeps run artifacts, as today.
- **A tag build**:
  - **It checks first, and stops on any mismatch.** The tag must be `a11y-v` plus `src/pc/a11y/VERSION`. `CHANGELOG.md`'s first version section must be that version. The tagged commit must be on `master`.
  - **It builds the Windows zip alone.** The Mac jobs are skipped on tags.
  - **It creates the release as a draft.** A draft is invisible to players and never "latest", so the release skill checks it and only then publishes it. The workflow's first real run is then safe to fail.
  - **It never replaces a release.** A run for a tag whose release already exists, draft or published, fails rather than replace it. A failed draft is deleted by hand before a re-run. A broken published release is fixed by the next patch.
- **The release notes** are put together by the workflow, in this order:
  1. A fixed header: "Plays online with Non-Visual Melee <version> and Melee-PC <base version>", the base version taken from `VERSION`; the requirements in a line or two (the player's own disc image, Windows 10 or later, a screen reader); and a link to `ACCESSIBILITY.md` at the tag (`blob/a11y-v<version>/ACCESSIBILITY.md`).
  2. The version's section of `CHANGELOG.md`, its heading dropped.
  3. A link to `CHANGELOG.md` at the tag.

  The notes need a clear heading structure: the maintainer reads them by heading.
- **Zip names stay as they are** (`NonVisualMelee-Windows-x86_64.zip`), with no version in them, so a link to the latest release's zip never changes.

### The launcher's update check

- **It follows fork releases**:
  - It reads the fork's releases (`ohylli/non-visual-melee`), skips any tag that is not `a11y-v<version>` (`ci-latest` among them), and compares versions with the fork version.
  - A release counts as an update only when it has a zip for this platform: `NonVisualMelee-Windows-x86_64.zip` on Windows, the Mac zips' names on a Mac. Until Mac zips ship in releases, a Mac build never hears of an update, and nothing special-cases the Mac.
  - "Open in browser" opens the fork's release page.
- **Its words**:
  - The messages name Non-Visual Melee, such as "Non-Visual Melee is up to date (0.1.0)."
  - An available update is named by its version, "0.1.1", not its tag.
- **The shown version**: where the launcher and the port menu show the app version, they show the fork version with the base version beside it, such as "0.1.0 (Melee-PC v0.4.2)". The build label is added where it fits.
- **How it attaches**:
  - Each base port line that changes is a one-line call into fork code, declared in `a11y_hooks.h` and marked with an `a11y` comment.
  - The fork decides the repository, the release to offer, the asset and the words.
  - The release-picking logic is fork code with unit tests on canned release lists, so the base port's `SemVer` and `select_best_asset` may be bypassed rather than bent.
  - The implementer keeps the number of changed base port lines small. Each one is a merge conflict waiting to happen.
- **Speech**: the launcher's reader already says the update card and the status lines. Its words follow the new text with no change, which a drive run confirms.

### The release skill

`.claude/skills/release/SKILL.md`, invoked by the maintainer. It is light:

1. It checks that `dev` is clean and pushed, and that Unreleased has entries.
2. It asks for a minor or a patch bump.
3. It renames Unreleased to the version and date, adds the "Plays online with" line, bumps `src/pc/a11y/VERSION`, and commits on `dev`.
4. It fast-forwards `master`, tags it and pushes `dev`, `master` and the tag.
5. It watches the tag's run.
6. It checks the draft release: the title, the notes, one Windows zip, and `readme.html` and `changelog.html` inside it (downloaded and listed, never run).
7. It publishes the draft once the maintainer agrees, and reports the release's link.

It has a hotfix variant, from `master`, that ends with `master` merged into `dev`. Verify is not part of it; the maintainer runs it before a release when they judge it worth it.

### Docs

- **CLAUDE.md**:
  - "Remotes and branches": `dev`, `master`, hotfixes and tags. It loses the line saying a `dev` branch is added after a first alpha.
  - The CI paragraph: what each trigger does.
  - "Docs": the changelog rule and the full-URL rule.
  - "Online compatibility": the fork version beside the unchanged app version.
  - "Accessibility status": the Launcher line names the update check following fork releases.
- **The `base-merge` skill** merges into `dev`.
- **The `code-review` skill** diffs branches against `dev`.
- **The `verify` skill** names `dev` where it names `master` as the branch `ci-latest` builds.
- **`ACCESSIBILITY.md`**:
  - Getting started names `readme.html` (this document) and `changelog.html` in the zip.
  - Online play says the release notes name the Melee-PC version a release plays with.
  - Each release's version is the fork's own.

## Open questions

For the implementer to find out:

1. Whether pandoc's standalone page reads well by heading in a browser with NVDA as it comes, or needs its table of contents left off or its style trimmed. The maintainer judges this in issue 07.
2. Whether the base port's `SemVer` can parse `a11y-v0.1.0` once the prefix is stripped, or the fork compares versions itself.
3. Where the launcher's shown version fits the build label without crowding the line.

## Issues

1. `issues/01-dev-branch.md`: the `dev` branch, `ci-latest` from it, and the docs and skills that name the working branch.
2. `issues/02-fork-version-and-changelog.md`: the version file, its define, the build label, the startup log line, `CHANGELOG.md` and its rules.
3. `issues/03-zip-documents.md`: `readme.html` and `changelog.html` in every zip. Blocked by 01, 02.
4. `issues/04-release-workflow.md`: the tag build, its checks, its notes. Blocked by 03.
5. `issues/05-launcher-update-check.md`: the update check and the shown version. Blocked by 02.
6. `issues/06-release-skill.md`: the release skill. Blocked by 04.
7. `issues/07-first-release.md`: the maintainer cuts 0.1.0 and checks it by ear. Blocked by 05, 06.

Docs are part of the issue they belong to. 01, 03 and 04 are chained because they all change `a11y.yml`.
