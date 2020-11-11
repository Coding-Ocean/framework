#ifndef font_HD
#define font_HD

#include "COLOR.h"
void initDebugStr();
void freeDebugStr();
void setDebugStr( const char* format, ... );
void drawDebugStr( COLOR c=COLOR(1,1,0,1) );
float fps();

#endif