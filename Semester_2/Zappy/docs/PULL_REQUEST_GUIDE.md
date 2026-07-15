# 🔀 Pull Request Guide (short version)

Objective: make PRs clear, quick to review, and easy to merge.

## 1. Before creating the PR

- Link the PR to an issue (`Closes #123`) except for **URGENT HOTFIX**.
- Create a clear branch name: `FIX/...`, `FEATURE/...`, `TECH/...`, `HOTFIX/...`, `DOC/...`.
- Make short, descriptive commits (`FIX: ...`, `FEATURE: ...`, `REFACTOR: ...`).
- Verify locally before pushing (`make`).

## 2. Create the PR

1. Open `Compare & pull request`.
2. Fill the template without leaving sections empty.
3. Assign reviewer(s) (Victor JOST).

## 3. Required sections

- **Summary**: 2–3 lines max.
- **Related issue**: `Closes #XXX` / `Related to #XXX`.
- **Affected mission(s)**: Check the relevant Zappy missions.
- **Change type**: FIX / FEATURE / REFACTOR / DOCS / TESTS / CI-CD / CHORE.
- **Tests and validation**: Commands run + result (manual check).
- **Non-regression**: Confirm no existing behavior is broken.
- **Boy Scout Rule**: Leave the code cleaner than before.
- **Documentation**: Indicate README / Changelog / docs modified, or state "none".

## 4. Review and merge

- The author fixes feedback in small commits and pushes again.
- Merge using **Squash and merge** or **Rebase and merge** (avoid merge commits).

## Quick checklist

- [x] Clear title
- [x] Short summary
- [x] Related issue
- [x] Tests/Build/Lint OK
- [x] Non-regression checked
- [x] Docs updated if necessary
- [x] No dead code / stray logs

---

## Need help?

- 📖 Read [ISSUES_GUIDE.md](ISSUES_GUIDE.md) to understand issues
- 🧪 Read [../README.md](../README.md) to understand the project

**Remember the Boy Scout Rule: leave the code cleaner than you found it! 🧹✨**
