# 2D-cmd-dungeoncrawler
2D cmd game made in C++, created for learning purposes.

Current win condition (2015-04-16):
-	Player enter the same space as an Exit.

Current lose condition (2015-04-16):
-	Player enter the same space as a Monster.

How the game currently functions (2015-04-16):
-	Draws the room based on the selected field width and length.
-	Each ASCII character represents a tile.
-	Back-end, each interacting element exist with an X and Y positional value.
-	Interacting elements are Monsters, Walls, Exits and the Player.
-	Non-Interacting elements include, but is not limited to, the floor, which does not have any positional values.
-	For any change made, such as movement, each tile is checked for interaction.
-	In the event of the Player or a Monster trying to enter the same space as a wall, it's positional value remains unchanged.

How I want the game to function and additional features (2015-04-16):
-	Back-end, map out positions of all interacting elements on a gridlike system and only check local changes to eliminate the runtime performance impact of checking every position for each change.
-	Entering an Exit leads to a new room, instead of being the win condition.
-	Pseudo-random, asymetrical generation of rooms.
-	Pseudo-random, asymetrical generation of inner walls, that does not cut the room in half.
-	Damage and health systems.
-	Items with various stats.

Known bugs (2015-04-16):
-	RandomizeMonsterMovement( WorldStruct &World ) not acting as it should (75% to stand still and 6,25% to go in each direction) but instead randomly selected Monsters always move and randomly selected Monsters stand still / rarely move.
