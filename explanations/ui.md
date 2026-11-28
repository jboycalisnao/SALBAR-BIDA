# `ui.h` & `ui.cpp` — Terminal Colors, Screen Clearing & Badges

## Plain-English Purpose
`ui.h` and `ui.cpp` are the **interior designers** of the terminal. Plain black-and-white console text is hard to read, so this module adds colors, clears old clutter off the screen, pauses so you can read tables, and prints neat colored tags like `[HIGH]` or `L1-EMERGENCY`.

---

## Naming Conventions
- **`Color::` Namespace**: Groups all ANSI escape-code strings (`Color::RED`, `Color::BOLD`, `Color::RESET`) so they don't collide with other variable names.
- **`print*` Functions**: Functions that immediately output formatted text to the screen (`printSuccess`, `printError`, `printWarning`, `printInfo`, `printHeaderBanner`).
- **`badge*` / `color*` Functions**: Functions that *return* a colored `string` so it can be embedded inside tables (`badgeVulnerability`, `badgePriority`, `badgeStatus`, `colorCode`, `colorOccupancy`).

---

## Actual Code & Explanation

### 1. ANSI Color Constants (`ui.h`)
Special character codes that tell the terminal to switch text color or style, and `RESET` to go back to normal:

```cpp
namespace Color {
    const std::string RESET         = "\033[0m";
    const std::string BOLD          = "\033[1m";
    const std::string DIM           = "\033[2m";
    const std::string RED           = "\033[31m";
    const std::string GREEN         = "\033[32m";
    const std::string YELLOW        = "\033[33m";
    const std::string CYAN          = "\033[36m";
    const std::string BRIGHT_RED    = "\033[91m";
    const std::string BRIGHT_GREEN  = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_MAGENTA= "\033[95m";
    const std::string BRIGHT_CYAN   = "\033[96m";
    const std::string BRIGHT_WHITE  = "\033[97m";
}
```

### 2. Console Setup, Screen Clearing & Pausing (`ui.cpp`)
`initConsoleUI()` enables color codes on Windows. `clearScreen()` wipes the terminal and moves the cursor to the top-left. `pauseConsole()` waits for Enter:

```cpp
void initConsoleUI() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#endif
    cout << "\033[2J\033[H" << flush;
}

void pauseConsole(const string& message) {
    cout << "\n" << Color::DIM << message << Color::RESET;
    cin.clear();
    string dummy;
    getline(cin, dummy);
}
```

### 3. Banners & Status Alerts (`ui.cpp`)
Draws the main top box and colored `[OK]`, `[!]`, `[WARN]`, `[i]` messages:

```cpp
void printHeaderBanner(const string& title, const string& subtitle, const string& facilityInfo) {
    cout << Color::BRIGHT_CYAN << "===========================================================================\n";
    cout << " " << Color::BOLD << Color::BRIGHT_WHITE << title << Color::RESET << "\n";
    if (!subtitle.empty()) {
        cout << " " << Color::DIM << Color::CYAN << subtitle << Color::RESET << "\n";
    }
    if (!facilityInfo.empty()) {
        cout << " " << Color::BOLD << Color::BRIGHT_WHITE << "Current Facility: " << Color::RESET << facilityInfo << "\n";
    }
    cout << Color::BRIGHT_CYAN << "===========================================================================" << Color::RESET << "\n";
}

void printSuccess(const string& message) {
    cout << Color::BOLD << Color::BRIGHT_GREEN << "[OK] " << Color::RESET << message << "\n";
}

void printError(const string& message) {
    cout << Color::BOLD << Color::BRIGHT_RED << "[!] " << Color::RESET << Color::BRIGHT_RED << message << Color::RESET << "\n";
}
```

### 4. Visual Badges (`ui.cpp`)
Wraps text labels in color codes so urgent items jump out visually:

```cpp
string badgeVulnerability(const string& level) {
    if (level == "HIGH") {
        return Color::BOLD + Color::BRIGHT_RED + "[HIGH]" + Color::RESET;
    } else if (level == "MODERATE") {
        return Color::BOLD + Color::BRIGHT_YELLOW + "[MODERATE]" + Color::RESET;
    } else {
        return Color::BOLD + Color::BRIGHT_GREEN + "[LOW]" + Color::RESET;
    }
}

string badgePriority(int priority) {
    switch (priority) {
        case 1: return Color::BOLD + Color::BRIGHT_RED + "L1-EMERGENCY" + Color::RESET;
        case 2: return Color::BOLD + Color::BRIGHT_YELLOW + "L2-HIGH" + Color::RESET;
        case 3: return Color::BOLD + Color::BRIGHT_CYAN + "L3-MODERATE" + Color::RESET;
        default: return Color::DIM + "L4-ROUTINE" + Color::RESET;
    }
}
```
