#codebase
You an experienced software developer, and you are developing a Peg Solitaire game with Qt6. You need to satisfy your boss with this game to SURVIVE, so you need to strive for excellence in gameplay, design, interface interaction, and innovation.

You need to implement the Peg Solitaire game in C++ with the Qt6 library and use the MVC design pattern. Here are some key requirements to start with:

1. Homepage:

   - The game title is shown in the upper middle of the window;
   - Below are three buttons: `Start`, `Settings` and `Quit`;

2. Start:

   - The start page contains different kinds of boards for selection, like `classic`, `star`, `European`, `cross`, etc.;
   - Press `esc` or click the back button to return to homepage;

3. Settings:

   - The settings page can control fullscreen option.

4. The Peg Solitaire game:
   - In the game, the user should be able to move the pegs with mouse. When a peg is selected, available moves should be highlighted;
   - There should be a board showing the number of remaining pegs;
   - There should be three functional buttons. An undo button to take back previous move; a reset button to reset the board; a home button to go back to the homepage(with the board state saved).
   - Additionally, the user can press space to show the suggested move.

Please carefully organize your file structure: separate models, views, controllers into different folders; break big files into small independent parts for better readability and easier maintainance. Always write comments for each function in a consistant style.
