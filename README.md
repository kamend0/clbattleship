# clbattleship
about half of Battleship played completely in terminal, written in C++ and taking advantage of OOP. one player hides the ships, then the other fires at coordinates to sink the hidden ships; one-player mode hides ships randomly.

Some issues:
- pretty clunky/rough around the edges/bloated; I think I could slim down the code more.
- I didn't mess with pointers, but I really should. they would probably simplify the logic and save a fair bit of memory. (specifically, passing around a pointer to the gameboard would probably be a good idea.)
- it's half the game. would like to make it the whole game. doing this on a command line interface would be difficult.
- it's hard. number of guesses needed to win, guessing randomly, hovers above 78. (http://www.datagenetics.com/blog/december32011/) current logic has 17 misses equating to a loss. (I feel like some sort of limit is necessary for this sort of half-version of the game.)
- I'd like to make it more fun!
