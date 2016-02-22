```
2D-cmd-dungeoncrawler
---------------------
A game made in C++ using no frameworks, created for learning purposes.

Explanation of all the files used:
- Game         - Overarching class that has all the required data components as member variables and acts as a controller and viewer.
- Dungeon      - Central data class that contain all the necessary information that the player observes, thus acting like a model.
- Vector2i     - Simple struct for representing a position in a grid. Contains two integers and a number of overloaded operators.
- BattleSystem - Contains everything required to stimulate combat, thus have all the components of the MVC design pattern.
- Player       - Contains necessary data to display player as well as inherit from Combatant to be usable in BattleSystem.
- Functions    - All the free functions used throughout the project.
- Enums        - All enums used throughout the project.
```
