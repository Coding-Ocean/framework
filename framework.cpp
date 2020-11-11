#include<windows.h>
#include<crtdbg.h>
#include"framework.h"
#include"window.h"
#include"graphic.h"
#include"debugStr.h"
#include"input.h"
#include"sound.h"

void initialize( const char* caption, int baseWidth, int baseHeight, int clientWidth, int clientHeight ){
    if( clientWidth == 0 || clientHeight == 0 ){
        clientWidth = baseWidth;
        clientHeight = baseHeight;
    }
    initWindow( caption, clientWidth, clientHeight );
    initGraphic( baseWidth, baseHeight, TRUE );
    initDebugStr();
    initInput();
    initSound();
    timeBeginPeriod( 1 );
   	setDeltaTime();
}

void initialize( const char* caption, int baseWidth, int baseHeight, WINDOW_MODE windowMode ){
    initWindow( caption );//fullscreen
    initGraphic( baseWidth, baseHeight, TRUE );
    initDebugStr();
    initInput();
    initSound();
    timeBeginPeriod( 1 );
   	setDeltaTime();
}

void initialize( const char* caption, WINDOW_MODE windowMode ){
    initWindow( caption );//fullscreen
    initGraphic( ClientWidth, ClientHeight, TRUE );
    initDebugStr();
    initInput();
    initSound();
    timeBeginPeriod( 1 );
   	setDeltaTime();
}

void finalize(){
    timeEndPeriod( 1 );
    freeSound();
    freeDebugStr();
    freeGraphic();
    _CrtDumpMemoryLeaks();
}

//この関数はユーザーが作る
void gmain();


int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, INT ){
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );	// メモリリークチェック
    gmain();
    finalize();
    return 0;
}