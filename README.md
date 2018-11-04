# Snake Game Multiplayer Project

## Dependencies

In order to be able to compile and run the game, please, install the follow packages on your linux system:

- **portaudio**. For Ubuntu systems, execute this line command on your terminal: **sudo apt-get install portaudio19-dev**

- **ncurses**. For Ubuntu systems, execute this line command on your terminal: **sudo apt-get install libncurses-dev**

## Running the Game

In order to run the game, in root folder type on the line command

<p align="center">
  <b>make server PORT=PORTNUMBER PLAYERS=N</b>
</p>

to run the server. You have to choose a avaiable __PORTNUMBER__ and the number of players __N__. After that, ou just have to run the client program in another terminal or another machine typing 

**make client IP=IP_TARGET PORT=PORTNUMBER**

where __IP_TARGET__ is the IP of the machine of the server. Ensure that all dependencies are installed. Please, before running the game, __maximize your terminal console__. The whole game is played at the terminal, so the performance depends of your terminal configurations too.

## Game Rules

The game starts with a snake with 4 body peaces of length. According as the snake eats its body grows. The borders of the screen can kill the snake, i.e, there are two ways to reach game over, which are when the snake crash against another snake or against the borders.

## Keyboard Map

- M/m: Turn on/off sound

- I/i: Increase background music volume

- D/d: Decrease background music volume

- ARROW KEYS: Move snake

- SPACE BAR: Speed up snake for a little piece of time

- ESC: Exit game

## Cleaning Game Space

In order to remove files created during the game and reset statistics, in root folder type on the line command **make clean**.














