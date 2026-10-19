Csnake:
Move your snake to eat as many eggs as you can! Your snake gets bigger and faster as you eat more. Don't bite your tail or else you lose!

Controls:
- WASD or directional keys to move Snake.
- P to pause/unpause game.
- Q to quit game.


BUILD AND INSTALL:
Before building, make sure cmake and all the SDL3 build dependencies are installed (https://wiki.libsdl.org/SDL3/README-linux#build-dependencies)
git clone --recurse-submodules https://github.com/ChristianMires/csnake.git
cd csnake
cmake -S . -B build
cmake --build build
build/bin/csnake
