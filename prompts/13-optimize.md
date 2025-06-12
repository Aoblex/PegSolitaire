#codebase

You an experienced software developer, and you are developing a Peg Solitaire game with Qt6.
`solveBoard` is using dfs to search for the solution.

The default backtracking algorithm, unfortunately, has an exceptionally long run-time, due to the many possible choices one may make at each step. We may, however, exploit symmetry in the problem.
Take the classic English board as an example: given any state, rotate the state 90 degrees four times, and we get 4 equivalent states, mirror these states and we get 8 equivalent states. They represent essentially the same position.

These boards are simply rotations or a flip followed by a rotation of each other. We will say that these boards are congruent to each other.

A consequence of backtracking is that there is an absolute certainty that congruent boards will, never-the-less, be solved multiple times, even it is determined previously that the board does not lead to a solution. The run-time of the default algorithm is significantly longer than this author was willing to wait (hours).

In order to exploit symmetry, there must be a means representing congruent boards by a unique identifier.

One solution is to convert board to a 49-bit number by traversing through the entries in each of eight orders corresponding to the eight symmetries, as shown in the provided figure.

Thus, these eight boards code the eight values:

      0011100001010011010111101101111111100111000011100
      0011100001110011001111011111110111100101000011100

      0011100001010011011111011111110011100111000011100
      0011100001010011010111011011111111100111000011100

      0011100001110011111111011011110101100101000011100
      0011100001010011110111111101111001100111000011100

      0011100001110011100111111101111101100101000011100
      0011100001110011111111101101110101100101000011100

We choose the number that is the smallest to represent this board, and all congruent boards will also result in the same unique identifier. In this case, that identifier is the fourth: 0011100001010011010111011011111111100111000011100. All of the above boards shown in Figure 5 will map to this unique identifier.

Thus, we create a set of identifiers, and each time we determine that a board leads to a non-solution, its identifier is stored in a set, and if that board or any similar board is visited, the backtracking algorithm halts.

The modified algorithm is as shown here:

With n pegs,
if there is only n = 1 peg left,
if it is in the center, return indicating we found a solution,
otherwise, we did not;
if this board is flagged as congruent to one that has been determined to not lead to a solution, return indicating we did not find one,
otherwise, for each possible jump,
modify the board to account for the jump and call the backtracking algorithm recursively, where
if the algorithm indicates success, record the jump and return indicating we found a solution,
otherwise, reset the board to the state it was in prior to making the jump; and
the loop will only finish if none of the possible jumps led to a solution, so flag that this board has been found not to find a solution and return indicating we did not find one.

Modify the codebase according to the algorithm described above.