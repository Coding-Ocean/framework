#pragma once

#define True msgProc()

void initialize( 
    const char* caption="Game", 
    int baseWidth=640, 
    int baseHeight=480, 
    int clientWidth=0, 
    int clientHeight=0 
);

enum WINDOW_MODE{ FULLSCREEN };

void initialize(
    const char* caption,
    int baseWidth, 
    int baseHeight,
    WINDOW_MODE windowMode
);

void initialize(
    const char* caption,
    WINDOW_MODE windowMode
);

void finalize();