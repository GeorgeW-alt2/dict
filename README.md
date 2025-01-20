# Real-time Spell Checker

A lightweight spell checker that monitors your typing in real-time and alerts you when words are misspelled. No external spell-checking libraries required - just uses a simple text file dictionary.

## Features

- Real-time keyboard monitoring
- Instant audio feedback for misspelled words
- Lightweight dictionary using simple text file
- Minimal memory footprint
- No external spell-checking libraries needed
- Works system-wide (all applications)

## Setup

### Prerequisites
- Windows operating system
- C++ compiler (Visual Studio or MinGW)
- Python 3.x (for dictionary creation)

### Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/realtime-spellchecker.git
cd realtime-spellchecker
```

2. Create the dictionary file:
```bash
python create_dictionary.py
```

3. Compile the C++ program:

Using Visual Studio:
```bash
cl spellchecker.cpp /EHsc
```

Or using MinGW:
```bash
g++ spellchecker.cpp -o spellchecker.exe
```

## Usage

1. Run the compiled executable:
```bash
spellchecker.exe
```

2. The program will run in the background and monitor your typing:
   - Beeps when it detects a misspelled word
   - Shows misspelled words in the console window
   - Press ESC to exit

### Customizing the Dictionary

The `dictionary.txt` file can be modified to add or remove words:
- One word per line
- Case-insensitive
- No punctuation
- Plain ASCII text

Example:
```
hello
world
computer
programming
```

## How it Works

The program uses Windows Low-Level Keyboard Hooks to:
1. Monitor keystrokes system-wide
2. Build words character by character
3. Check completed words against the dictionary
4. Provide audio feedback for misspellings

The dictionary is loaded into a hash set for O(1) lookup performance.

## Technical Details

- Uses Windows API for keyboard hooks
- Unordered set for dictionary storage
- No external dependencies
- Minimal CPU and memory usage

## Security Note

Since this program monitors keyboard input, some antivirus software might flag it. The source code is open for inspection to verify its safety.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## Limitations

- Windows-only support currently
- English dictionary by default
- Cannot spell-check passwords in secure input fields
- Limited to ASCII characters

## Future Improvements

- [ ] Add support for multiple languages
- [ ] Create custom dictionaries for specific domains
- [ ] Add visual feedback options
- [ ] Implement word suggestions
- [ ] Add Linux/Mac support

## License

MIT License - see LICENSE file for details

## Acknowledgments

- Word list based on common English vocabulary
- Inspired by real-time spell checkers in IDEs

---
Created by [Your Name] - Feel free to contact me for any questions!
