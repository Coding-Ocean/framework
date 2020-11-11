#include<windows.h>
#include<crtdbg.h>
#include<math.h>
#include"framework.h"
#include"window.h"
#include"graphic.h"
#include"debugStr.h"
#include"input.h"
#include"sound.h"
#include"VECTOR3.h"
#include"MATRIX.h"

#define card double
#define program() void gmain()
#define loop() while(ultraBasicMsgProc())
#define repeat(N) for( int iiiiiiii = 0; iiiiiiii < N; iiiiiiii++ )

void window( double width=640, double height=480, const char* title="GAME" );

int jump( card& y, card floorY=0 );