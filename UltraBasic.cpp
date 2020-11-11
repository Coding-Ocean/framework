#include "UltraBasic.h"

//ここから、このライブラリのユーザーが使用する関数
void window( double width, double height, char* title ){
    initWindow( title, (int)width, (int)height );
    initGraphic( (int)width, (int)height, TRUE );
    initInput();
    initDebugStr();
    initSound();
    timeBeginPeriod( 1 );
   	setDeltaTime();
}

int jumpFlag = 0;
card Dy = 0;
card Gravity = -0.8;
int jump( card& y, card floorY ){
    if( jumpFlag == 0 && isTrigger( KEY_Z ) ){
        jumpFlag = 1;
        Dy = 20;
    }
    if( jumpFlag ){
        y += Dy;
        Dy += Gravity;
        if( y < floorY ){
            y = floorY;
            jumpFlag = 0;
        }
    }
	else {
		y = floorY;
	}
	return jumpFlag;
}