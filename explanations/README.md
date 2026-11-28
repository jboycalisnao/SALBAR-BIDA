# SALBAR Codebase Explanations (Layman's Guide)

Welcome to the plain-English guide for the **SALBAR (Barangay Disaster Relief Management System)** codebase! Every module in this project has its own `.md` file explaining:
1. **What it does** in everyday terms.
2. **Naming conventions** used (why variables, functions, and constants are named the way they are).
3. **Every function & logic block** explained step by step.

---

## Module Guides

| Guide File | Source Files Covered | What It's About |
| :--- | :--- | :--- |
| [models.md](./models.md) | `models.h` | The "blueprints" (structs) for Families, Centers, Requests, and Evacuees, plus system limits and filenames. |
| [salbar.md](./salbar.md) | `salbar.cpp` | The "front desk" / main program loop that starts the app, draws the main menu, and routes you to the right module. |
| [family_module.md](./family_module.md) | `family_module.h`, `family_module.cpp` | **Case 1 & Case 2**: Registering new families and viewing the active family directory table. |
| [center_module.md](./center_module.md) | `center_module.h`, `center_module.cpp` | **Case 3**: Adding, viewing, updating, deleting evacuation centers (including auto-removing evacuees under them), and choosing the active center. |
| [request_module.md](./request_module.md) | `request_module.h`, `request_module.cpp` | **Case 4**: Submitting relief requests, sorting them by urgency, and updating request statuses. |
| [record_module.md](./record_module.md) | `record_module.h`, `record_module.cpp` | **Case 5**: Editing family/evacuee details, soft-deleting (disabling/re-enabling) families, and deleting evacuees. |
| [analytics.md](./analytics.md) | `analytics.h`, `analytics.cpp` | **Case 6**: Scoring family vulnerability and building the 2D demographic summary report. |
| [persistence.md](./persistence.md) | `persistence.h`, `persistence.cpp` | **Case 7 & Auto-Save**: Saving and loading all records to `.txt` files so nothing is lost when you close the app. |
| [operations.md](./operations.md) | `operations.h`, `operations.cpp` | Behind-the-scenes helpers for recursive searching, recursive counting, recursive sorting, and displaying formatted records. |
| [utils.md](./utils.md) | `utils.h`, `utils.cpp` | Input safety guards: stopping empty inputs, checking number ranges, turning `1` into `FAM-0001`, and handling `0`/`exit` cancellations. |
| [ui.md](./ui.md) | `ui.h`, `ui.cpp` | Terminal visuals: ANSI colors, clearing the screen so menus stay at the top, pausing, and drawing badges like `[HIGH]`. |

---

## Global Naming Conventions Used Across SALBAR

1. **`UPPER_SNAKE_CASE`** (e.g., `MAX_FAMILIES`, `FILE_CENTERS`):
   - Used for **constants** — fixed numbers or filenames that never change while the program is running.
2. **`PascalCase`** (e.g., `Family`, `EvacuationCenter`, `ReliefRequest`):
   - Used for **structs** (custom data types / blueprints) and namespaces (`Color`).
3. **`camelCase`** (e.g., `familyCount`, `handleFamilyRegistration`, `recursiveSearchFamily`):
   - Used for **variables** and **functions**.
   - **Verb-first functions**: Functions start with an action word describing what they do (`handle...` for menu screens, `prompt...` for asking the user, `print...` for showing text, `recursive...` for functions that call themselves).
4. **Header Guards** (`#ifndef SALBAR_..._H` / `#define SALBAR_..._H` / `#endif`):
   - Placed at the top and bottom of every `.h` file so C++ doesn't accidentally include the same file twice and cause duplicate-definition errors.
