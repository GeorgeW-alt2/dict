#include <windows.h>
#include <iostream>
#include <string>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <regex>

// Global variables
HHOOK keyboardHook;
std::string currentWord;
std::string currentSentence;
std::unordered_set<std::string> informalWords = {
    // Contractions
    "ain't", "aren't", "can't", "couldn't", "didn't", "doesn't", "don't", "hadn't",
    "hasn't", "haven't", "he's", "here's", "how's", "i'd", "i'll", "i'm", "i've",
    "isn't", "it's", "let's", "shouldn't", "that's", "there's", "they're", "wasn't",
    "we're", "weren't", "what's", "where's", "who's", "won't", "wouldn't", "you're",
    "you've", "y'all", "would've", "should've", "could've", "might've", "must've",
    "we've", "they've", "i'ma", "gonna", "wanna", "gotta", "lemme", "gimme",

    // Common Slang
    "sup", "bro", "bruh", "bruv", "mate", "dude", "dawg", "homie", "fam", "sis",
    "bestie", "bff", "buddy", "kiddo", "folks", "peeps", "guy", "guys", "yep", "nope",
    "nah", "yeah", "yup", "cool", "awesome", "sweet", "rad", "sick", "lit", "fire",
    "dope", "epic", "legit", "basic", "salty", "tea", "shade", "yeet", "flex", "cap",
    "no cap", "sus", "goat", "mood", "vibe", "slay", "bet", "bussin", "based", "mid",

    // Casual Words/Expressions
    "stuff", "thing", "thingy", "kinda", "sorta", "maybe", "prob", "dunno", "whatever",
    "anyways", "anyway", "like", "literally", "basically", "pretty", "really", "very",
    "just", "actually", "totally", "absolutely", "surely", "guess", "probably", "ok",
    "okay", "sort of", "kind of", "a lot", "lots", "tons", "ton", "bunch", "big",
    "huge", "massive", "way", "super", "extra", "real", "totally", "completely",

    // Internet/Text Speak
    "lol", "omg", "wtf", "idk", "btw", "fyi", "asap", "thx", "tx", "ur", "u", "r",
    "gr8", "m8", "l8r", "b4", "2day", "4u", "tbh", "imo", "imho", "afaik", "aka",
    "thru", "tho", "plz", "pls", "dm", "pm", "tmrw", "rn", "af", "smh", "nvm",
    "ttyl", "brb", "gtg", "hbu", "hru", "iirc", "irl", "jk", "lmao", "lmfao",
    "rofl", "stfu", "tba", "tbd", "tldr", "w/", "w/o", "wb", "wbu", "wyd", "xoxo",

    // Filler Words
    "well", "so", "um", "uh", "er", "like", "you know", "i mean", "right", "mhm",
    "hmm", "erm", "eh", "oh", "ah", "mmm", "basically", "literally", "actually",
    "seriously", "honestly", "apparently", "presumably", "possibly", "probably",
    "personally", "generally", "usually", "typically", "basically", "essentially",

    // Casual Intensifiers
    "super", "really", "very", "totally", "absolutely", "completely", "literally",
    "seriously", "definitely", "basically", "actually", "pretty", "quite", "rather",
    "somewhat", "kinda", "sorta", "fairly", "highly", "surely", "clearly", "obviously",
    "apparently", "supposedly", "presumably", "practically", "virtually", "nearly",

    // Modern Internet Slang
    "poggers", "pog", "sheesh", "copium", "hopium", "ratio", "based", "cringe",
    "cursed", "blessed", "yikes", "oof", "mood", "vibe", "aesthetic", "hits different",
    "slaps", "periodt", "finna", "lowkey", "highkey", "deadass", "facts", "cap",
    "no cap", "bread", "sauce", "tea", "salty", "shook", "sus", "zaddy", "simp",

    // Casual Verbs
    "gimme", "lemme", "gonna", "wanna", "gotta", "tryna", "coulda", "shoulda",
    "woulda", "ain't", "dunno", "hafta", "supposta", "needa", "oughta", "bust",
    "cop", "yeet", "yoink", "vibe", "hit up", "hang out", "chill", "crash",
    "bounce", "book it", "dip", "ghost", "flex", "spill",

    // Questionable Grammar
    "ain't", "y'all", "yall", "ima", "imma", "innit", "whatcha", "ya", "yer",
    "em", "nuthin", "sumthin", "dunno", "gonna", "gotta", "wanna", "kinda",
    "sorta", "coulda", "shoulda", "woulda", "ain't", "dontcha", "gotcha",
    "watcha", "betcha", "bout", "'bout", "outta", "tryna",

    // Modern Work Slang
    "asap", "fyi", "tbd", "tba", "wfh", "ooo", "eod", "cob", "nom", "tgif",
    "mins", "sec", "convo", "sesh", "mtg", "prep", "admin", "docs", "stats",
    "info", "pic", "pics", "app", "tech", "temp", "demo", "dev", "prod",
    "sync", "async", "blockers", "touch base", "circle back", "deep dive"
};

// Informal sentence starters
std::unordered_set<std::string> informalStarters = {
    "so", "like", "well", "yeah", "hey", "look", "listen", "okay", "basically",
    "actually", "anyway", "plus", "also", "btw", "oh", "um", "uh", "right",
    "wow", "omg", "hmm", "honestly", "literally", "seriously", "okay so",
    "alright so", "but like", "and then", "basically", "apparently", "honestly",
    "technically", "theoretically", "practically", "virtually", "generally",
    "usually", "typically", "normally", "naturally", "obviously", "clearly",
    "surely", "certainly", "probably", "possibly", "maybe", "perhaps",
    "surprisingly", "interestingly", "unfortunately", "fortunately", "sadly",
    "hopefully", "thankfully", "luckily", "honestly", "frankly", "admittedly"
};

bool isInformalPattern(const std::string& text) {
    // Check for repeated letters (e.g., "sooo", "yeahhh")
    std::regex repeatedLetters("(.)\\1{2,}");
    if (std::regex_search(text, repeatedLetters)) {
        return true;
    }

    // Check for all caps (excluding acronyms)
    if (text.length() > 3 &&
        std::all_of(text.begin(), text.end(), ::isupper)) {
        return true;
    }

    // Check for number substitutions (e.g., "gr8", "l8r")
    std::regex numberSubstitutions("\\w*\\d+\\w*");
    if (std::regex_search(text, numberSubstitutions)) {
        return true;
    }

    return false;
}

void checkFormality(const std::string& word, bool isSentenceStart = false) {
    std::string lowerWord = word;
    std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);

    bool isInformal = false;

    // Check if it's an informal word
    if (informalWords.find(lowerWord) != informalWords.end()) {
        isInformal = true;
    }
    // Check if it's an informal sentence starter
    else if (isSentenceStart &&
             informalStarters.find(lowerWord) != informalStarters.end()) {
        isInformal = true;
    }
    // Check for informal patterns
    else if (isInformalPattern(word)) {
        isInformal = true;
    }

    if (isInformal) {
        Beep(1000, 200);  // Beep at 1000Hz for 200ms
        std::cout << "\nInformal usage detected: \"" << word << "\"" << std::endl;
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
            DWORD keyCode = kbStruct->vkCode;

            // Check for escape key
            if (keyCode == VK_ESCAPE) {
                PostQuitMessage(0);
                return 1;
            }

            BYTE keyState[256] = {0};
            GetKeyboardState(keyState);

            char buffer[2] = {0};
            UINT scanCode = MapVirtualKey(keyCode, MAPVK_VK_TO_VSC);
            int result = ToAscii(keyCode, scanCode, keyState, (LPWORD)buffer, 0);

            if (result == 1) {
                char c = buffer[0];

                if (std::isalpha(c) || c == '\'') {
                    currentWord += c;
                    currentSentence += c;
                }
                else {
                    // Check if this is the start of a new sentence
                    bool isSentenceStart = currentSentence.empty() ||
                        std::regex_match(currentSentence, std::regex("^\\s*$"));

                    if (!currentWord.empty()) {
                        checkFormality(currentWord, isSentenceStart);
                        currentWord.clear();
                    }

                    // Handle sentence endings
                    if (c == '.' || c == '!' || c == '?') {
                        currentSentence.clear();
                    }
                    else {
                        currentSentence += c;
                    }
                }
            }
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

int main() {
    SetConsoleTitle("Formality Checker");
    std::cout << "Formality Checker Active" << std::endl;
    std::cout << "Will beep when informal language is detected" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;

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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
