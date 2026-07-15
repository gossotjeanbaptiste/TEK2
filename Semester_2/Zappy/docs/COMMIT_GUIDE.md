# Commit convention

This project follows a simple and readable commit convention to ease history tracking, code review, and quick understanding of changes.

## General format

```txt
[TYPE] Short description of the change
```

Examples:
```txt
[ADD] Add resource management on the map
[FIX] Fix player movement bug
[HOTFIX] Urgent fix for crash on startup
[DEL] Remove obsolete GUI rendering code
```

---

## Commit types

### `[ADD]`

Use for adding new features, logic, or useful files.

Examples:

- Adding a command
- Adding a rendering system
- Adding a module
- Adding documentation

---

### `[FIX]`

Use for fixing a bug or incorrect behavior without adding new features.

Examples:

- Fixing a calculation
- Fixing a display
- Fixing a logic error
- Fixing a synchronization issue

---

### `[HOTFIX]`

Use for urgent fixes, applied quickly to unblock the project or correct a critical error.

Examples:

- Crash on startup
- Blocking server error
- Critical bug preventing compilation or execution

---

### `[FEATURE]`

Use when adding a new feature not required by the project specification.

Examples:

- Adding VR support
- Training a custom AI
- Adding a mini-game

---

### `[DEL]`

Use for removing code, files, functions, or parts that are no longer needed.

Examples:

- Removing an old method
- Removing an unused file
- Removing an obsolete test

---

### `[REFACTOR]`

Use when reorganizing or improving code without changing its behavior.

Examples:

- Simplifying a function
- Splitting a large file into modules
- Improving readability
- Cleaning up structure

---

### `[DOC]`

Use for any documentation-related changes.

Examples:

- Adding a `README`
- Updating an installation guide
- Adding API documentation
- Fixing a project convention

---

### `[TEST]`

Use when commits concern tests.

Examples:

- Adding unit tests
- Modifying existing tests
- Improving coverage
- Fixing a broken test

---

### `[CHORE]`

Use for ancillary tasks that do not directly change business logic.

Examples:

- Updating dependencies
- Project configuration
- Modifying `.gitignore`
- Miscellaneous cleanup

---

## Rules to follow

- The type must always be uppercase inside brackets.
- The message should be short, clear, and precise.
- A commit should ideally contain a single main idea.
- Avoid vague messages like `update`, `fix`, `test`, or `changes`.
- Start with the type, then write an understandable description.

---

## Good examples

```txt
[ADD] Add server argument parsing
[FIX] Fix collision bug between two players
[DEL] Remove old logging system
[REFACTOR] Separate networking logic from game logic
[DOC] Update GUI protocol documentation
[HOTFIX] Urgent fix for crash on connection
```

---

## Bad commit example

```txt
update
fix bug
changes
```
These messages are too vague and do not help understanding project history quickly.