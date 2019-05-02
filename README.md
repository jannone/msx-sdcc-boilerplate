# MSX SDCC Boilerplate

This is a template project for building games on the MSX computer, in the format of a 40kb ROM (cartdrige).
It includes a barebones library for accessing BIOS and VDP resources.
It also includes a simple game loop to run your code within video frames.

To start implementing, check the comments on the `game.c` file.

# Compiling

You can compile directly in your local system, or use a docker image that already contains all the tools to compile

## Compiling in your local system

### Prerequisites

You need to have in your PATH these programs: `make`, `dd`, `rm`, `sed`,
`gawk`. Linux users always have them installed, but if you work on
Windows find them with Google (search for "UnxUtils").

You also need to have SDCC utilities in your PATH and to set the variable
`SDCC_HOME`.

### Start the compiler

Just run `make`
If you're using Windows, please adapt the file `Makefile` to point to your compiling tools.

## Compiling with Docker

If you don't want to install all the tools, you can install just Docker and then:

  * `docker run -v $(pwd):/mnt/myprogram fr3nd/sdcc /bin/bash -c "cd /mnt/myprogram; make"`

This will generate a ROM file in your `dist` dir.

# Running on a Mac

Use openMSX:
`open -a openMSX --args $(pwd)/dist/program.rom`
