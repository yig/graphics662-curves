# Curves

## Installing dependencies

The C++ code you write is compiled via [emscripten](https://kripken.github.io/emscripten-site/) into JavaScript that can run in the browser. Emscripten is the one-and-only prerequisite (other than a web browser).

1. Install [emscripten](https://kripken.github.io/emscripten-site/docs/getting_started/downloads.html).
2. Follow the instructions there for your platform to make the program `emcc` accessible on the command line.

## Compiling

If you have the program `make` installed (macOS and other unix users likely will), then you can simply type:

    make

Otherwise, the following line will compile all the code:

    emcc -Wall -Werror --bind -I. -O2 --memory-init-file 0 -o curvelib.html CurveBridge.cpp Curve.cpp CurveFunctions.cpp

## Writing

Fill in the functions in the file `CurveFunctions.cpp`. That's the only file you need to change.

## Debugging

You won't be able to set breakpoints in your C++ code, but you can use the following functions to log messages:

    // Stop execution and print an error with the current file and line number if the condition is false.
    assert( condition );
    
    // Pop up a dialog box with the text inside.
    jsAlert( string );
    
    // Print a message to the browser's console (in black, yellow, or red).
    jsLog( string );
    jsWarn( string );
    jsError( string );

You may also find the standard C++ helper function `std::to_string( int or float )` useful.
