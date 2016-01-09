# 2D-cmd-dungeoncrawler
A game made in C++, using no frameworks, created for learning purposes.

Explanation of all the classes used:
- Dungeon: The central class that contain all the necessary data for the game. It has a number of private member functions to either generate data or read previously saved data from a text file. The majority of functions are only called once or left unused per instance of Dungeon. It exposes only single elements for reading, which is for displaying the dungeon.
- Vector2i: Simple struct for representing a position in a grid. Contains two integers and a number of overloaded operators.
- Entity: The base class for all 'things' ingame. Contain the position and portrait of said thing, where the portrait is a char.
- Wall: Inherit from Entity, with no extra data or functions. Used to represent an obstacle which the player cannot move to.
- Door: Inherit from Entity, with no extra data or functions. Used to represent the exit out of the dungeon and is currently the win condition.
- Step: Inherit from Entity, with no extra data or functions. Used to represent a hidden path which obstacles cannot be placed upon, to ensure that the player can reach a door.
- Character: Inherit from Entity, in addition to having functions for moving in a grid, as well as function for attacking another character in a (very) simple battle system.
- Player: Inherit from Character, in addition to containing the line of sight the player has in a dungeon, which is used when displaying the dungeon.
- Monster: Inherit from Character, with no extra data or functions. Use the moving functions from Character to move randomly throughout the dungeon.
- RNG: Single function for generating a random number based on the inputted range.
- Enums: Contains three enums (Orientation, GameStatus, GameType) and the struct Portrait with static const char for displaying entities.
- AStarAlgorithm: An implementation of the A* path finding algorithm, used to generate a hidden path (of Steps).
- IO: Contains static functions for all Input and Output used throughout the game. The purpose of seperating all IO like this is so incase I decide to port the game to framework like ncurses or another GUI, making the process of porting easier (hopefully).
