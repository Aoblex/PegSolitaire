# Peg Solitaire Game

[![Windows Release](https://github.com/yourusername/solitaire/actions/workflows/windows-release.yml/badge.svg)](https://github.com/yourusername/solitaire/actions/workflows/windows-release.yml)
[![CI Build](https://github.com/yourusername/solitaire/actions/workflows/ci.yml/badge.svg)](https://github.com/yourusername/solitaire/actions/workflows/ci.yml)

A beautiful and modern Peg Solitaire game built with Qt6 and C++, featuring multiple game modes, intelligent strategy hints, and an intuitive user interface.

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
- **ESC**: Return to previous screen

## 🏗️ Building from Source

### Prerequisites
- **Qt6** (6.5.3 or later)
- **CMake** (3.16 or later)
- **C++17** compatible compiler
- **Visual Studio 2019/2022** (Windows)

### Build Steps

```bash
# Clone the repository
git clone https://github.com/yourusername/solitaire.git
cd solitaire

# Configure with CMake
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --config Release

# Run the game
.\build\Release\PegSolitaire.exe
```

## 📁 Project Structure

```
solitaire/
├── controllers/     # Game logic controllers (MVC pattern)
├── models/         # Data models and game state
├── views/          # UI components and layouts
├── utils/          # Utility classes and helpers
├── resources/      # Icons, images, and Qt resources
└── prompts/        # Development documentation
```

## 🚀 Releases

### Automated Releases
This project uses GitHub Actions for automated building and releasing:

- **CI/CD Pipeline**: Automatic builds on every push/PR
- **Windows Releases**: Tagged versions create Windows executables
- **Portable Package**: No installation required - just download and run!

### Download Latest Release
1. Go to the [Releases](https://github.com/yourusername/solitaire/releases) page
2. Download `PegSolitaire-Windows-x64-v*.zip`
3. Extract and run `PegSolitaire.exe`

### Creating a New Release
To create a new release, push a tag:

```bash
git tag v1.0.0
git push origin v1.0.0
```

This will automatically trigger the release workflow and create a new GitHub release with the compiled Windows executable.

## 🔧 Development

### Architecture
This project follows the **Model-View-Controller (MVC)** design pattern:

- **Models**: Game board state, peg positions, game rules
- **Views**: UI components, page layouts, visual elements  
- **Controllers**: User interaction handling, game logic coordination

### Key Components
- **BoardController**: Manages game logic and peg movements
- **StrategyWorker**: AI strategy calculation in background thread
- **GameView**: Main game interface with board visualization
- **LoadingCircle**: Animated loading indicator for strategy calculations

## 🐛 Troubleshooting

### Common Issues

**Game won't start:**
- Ensure you have Visual C++ Redistributable installed
- Check that all Qt6 DLLs are present in the same directory

**Strategy hints are slow:**
- This is normal for complex board states
- The loading circle indicates the AI is working

**Keyboard controls not working:**
- Make sure the game window has focus
- Try clicking on the game board first

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 🎯 Roadmap

- [ ] Additional board layouts
- [ ] Online multiplayer mode
- [ ] Achievement system
- [ ] Themes and customization
- [ ] Mobile version (Android/iOS)
- [ ] Tournament mode

---

**Enjoy playing Peg Solitaire!** 🎲✨
