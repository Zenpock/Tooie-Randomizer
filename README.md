# Banjo Tooie Randomizer
This is a randomizer project build in C++ intended for use with the US version of Banjo Tooie

## How to play
  - Open the TooieRando.exe found in the release folder
  - Enter a randomization seed or leave blank to get the default seed
  - Click "Randomize Original Rom"
  - Select your US Banjo Tooie rom
  - The program may freeze (I didn't want to deal with threads)
  - A prompt should open saying "Randomization Complete!" at which point you will be prompted to where to save your game.
  - From this point you can put in a different seed and click "Randomize" then "Save Rom" if you want a different seed

### Some Information about how this randomizer works
  - Collectable Items have been randomized across the whole game (Jiggies, Glowbos Mega Included, Honeycombs, Cheato Pages, Boggy Fish, Tickets, Doubloons, and Jinjos)
  - Moves have been randomized between any of the silos in the game (the prices remain the same for each location but the moves do not)
  - All Silos can be used by any character so you do not need to be the correct character to learn a move.
  - There is no logic to where items are placed so be aware it is possible for you to get stuck.
  - Notes are randomized within their world so all notes that were in a level should still be somewhere within that level (unless I messed it up)
  - Reward Items can only be the following items (Jiggies, Glowbos, Honeycombs, Cheato Pages, Tickets, Doubloons, and Jinjos)
### Known Issues
  - Waiting long enough on the title screen will crash (I do not know what it is trying to do but just dont be there)
  - Some Items are not randomized due to difficulty in Randomizing them (Jade Snake, Dinosaur Family, Robot Fight GI Inside, Column Jiggy, Pawno, Warming the water)
  - Reward items may spawn in slightly different places than in the original game. This is due to the way jiggies are handled in the original code for rewards as opposed to everything else.
  - Multiplayer maps will crash your game (I removed them early in the project for space because I was scared to shift assets around DO NOT TRY TO GO TO THEM)
### Future Features
  - Moves Item (An Item that gives an associated move)
  - More Options (Keeping Items in Level)
  - Stop and Swap (Randomizing the game paks is disabled so they will all be in their vanilla positions)

Since this tool is a heavily modified version of a modding tool GEDecompressor this randomizer can also work as a more specialized Banjo Tooie mod tool.
Some of the features I've changed 
  - Right clicking on any script asset in the asset window will allow you to edit the allocated size for the given script this must be done in intervals of 4 otherwise the asset will not be seen as valid.
  - Double clicking any asset in the list will open the associated file in a text editor.
