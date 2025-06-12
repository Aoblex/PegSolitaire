#codebase

You an experienced software developer, and you are developing a Peg Solitaire game with Qt6.
Now I want you to fix the keyboard selection logic to prevent from a bug.
Here is an example:

(0, 0), (0, 1), (0, 2), (0, 3), (0, 4),
(1, 0), (1, 1), (1, 2), (1, 3), (1, 4),
(2, 0), (2, 1), (2, 2), (2, 3), (2, 4),
(3, 0), (3, 1), (3, 2), (3, 3), (3, 4),
(4, 0), (4, 1), (4, 2), (4, 3), (4, 4),

if the first row and the first column pegs are all empty except for (0, 0), then when (0, 0) is selected, the player can not move peg focus by pressing WSAD.

Fix this problem.