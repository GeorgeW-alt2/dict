#include <windows.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <fstream>
#include <algorithm>

// Global variables
HHOOK keyboardHook;
std::string currentWord;
std::unordered_set<std::string> dictionary;

// Load dictionary from text file
bool loadDictionary(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open dictionary file: " << filename << std::endl;
        return false;
    }

    std::string word;
    while (std::getline(file, word)) {
        // Remove carriage return if present
        if (!word.empty() && word.back() == '\r') {
            word.pop_back();
        }
        // Convert to lowercase and add to dictionary
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        dictionary.insert(word);
    }

    file.close();
    std::cout << "Loaded " << dictionary.size() << " words" << std::endl;
    return true;
}

// Function to check if word is spelled correctly
bool checkSpelling(const std::string& word) {
    if (word.empty()) return true;

    // Convert to lowercase for checking
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    return dictionary.find(lowerWord) != dictionary.end();
}

// Keyboard hook callback function
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD keyCode = kbStruct->vkCode;

            // Convert virtual key to character
            BYTE keyState[256] = {0};
            GetKeyboardState(keyState);

            char buffer[2] = {0};
            UINT scanCode = MapVirtualKey(keyCode, MAPVK_VK_TO_VSC);
            int result = ToAscii(keyCode, scanCode, keyState, (LPWORD)buffer, 0);

            if (result == 1) {
                char c = buffer[0];

                // If character is a letter, add to current word
                if (std::isalpha(c)) {
                    currentWord += c;
                }
                // If space or punctuation, check the word
                else if (!currentWord.empty()) {
                    if (!checkSpelling(currentWord)) {
                        // Beep for misspelled word
                        Beep(1000, 200);  // 1000Hz for 200ms
                        std::cout << "\nMisspelled word: " << currentWord << std::endl;
                    }
                    currentWord.clear();
                }
            }

            // Check for ESC key to exit
            if (keyCode == VK_ESCAPE) {
                PostQuitMessage(0);
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main() {
    std::cout << "Simple Spell Checker" << std::endl;

    // Load dictionary
    if (!loadDictionary("dictionary.txt")) {
        std::cerr << "Failed to load dictionary" << std::endl;
        return 1;
    }

    std::cout << "Type normally - you'll hear a beep for misspelled words" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;

    // Install keyboard hook
    keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandle(NULL),
        0
    );

    if (!keyboardHook) {
        std::cerr << "Failed to install keyboard hook" << std::endl;
        return 1;
    }

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    UnhookWindowsHookEx(keyboardHook);

    return 0;
}
