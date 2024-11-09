Banjo Tooie Randomizer
This is a randomizer project build in C++ intended for use with the US version of Banjo Tooie

(This Randomizer is not currently playable as the patch to the game's code to allow for randomization has not yet been finished)

How to play
  Open the application .exe
  Click "Load/Decompress Game"
  Select the Banjo Tooie US rom
  Click "Load Objects" (the program will freeze until it has completed as I did not want to deal with threads)
  Enter a randomization seed or leave blank to get the default seed
  Click "Randomize" (the program will freeze until it has completed as I did not want to deal with threads)
  Once the program is completed click save rom and you can save your edited rom wherever you would like.

Known Issues
  - Reward items may spawn in slightly different places than in the original game. This is due to the way jiggies are handled in the original code for rewards as opposed to everything else.
  - Multiplayer maps will crash your game (I removed them early in the project for space because I was scared to shift assets around DO NOT TRY TO GO TO THEM)
Future Features
  - Moves Item (An Item that gives an associated move)
  - More Options (Keeping Items in Level)
  - Stop and Swap (Randomizing the game paks is disabled)

Since this tool is a heavily modified version of a modding tool GEDecompressor this randomizer can also work as a more specialized Banjo Tooie mod tool.
Some of the features I've changed 
  - Right clicking on any script asset in the asset window will allow you to edit the allocated size for the given script this must be done in intervals of 4 otherwise the asset will not be seen as valid.
  - Double clicking any asset in the list will open the associated file in a text editor.
