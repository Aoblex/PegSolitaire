# Peg Solitaire Game

![Peg Solitaire](./demo/icon.ico)

A beautiful and modern Peg Solitaire game built with Qt6 and C++, featuring multiple game modes, intelligent strategy hints, and an intuitive user interface.

## 📁 File Structure Explanation
TODO

## 🎮 Features

- **Multiple Game Modes**: English (Classic), Diamond, Square, Anti-Peg, and Endgame
- **Intelligent Strategy**: Press Space for AI-powered move suggestions
- **Intuitive Controls**: Mouse and keyboard support
- **Modern UI**: Clean, responsive interface with consistent styling
- **Game States**: Win/lose detection with replay options
- **Fullscreen Mode**: Immersive gaming experience
- **Loading Animations**: Visual feedback during strategy calculations

## 🕹️ Game Modes

### Classic Boards
- **English**: Traditional 33-hole cross-shaped board
- **Diamond**: Diamond-shaped board layout
- **Square**: Square grid board

### Special Modes  
- **Anti-Peg**: Reverse objective - try to have as many pegs as possible
- **Endgame**: Randomly generated puzzles with guaranteed solutions

## 🎯 Controls

### Mouse Controls
- **Click**: Select a peg
- **Click again**: Move selected peg to valid position

### Keyboard Controls
- **WASD**: Navigate between pegs
- **Arrow Keys**: Move selected peg in direction
- **Space**: Show strategy hint

## 🏗️ Building from Source

### Prerequisites
- **Qt6** (6.5.3 or later)
- **CMake** (3.16 or later)
- **C++17** compatible compiler
- **Visual Studio 2019/2022** (Windows)

### Build Steps

```bash
# Clone the repository
git clone https://github.com/Aoblex/PegSolitaire.git
cd PegSolitaire 

# Configure with CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --config Release

# Run the game
.\build\Release\PegSolitaire.exe
```