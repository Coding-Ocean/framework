#include <string>
#include "graphic.h"
#include "debugStr.h"
#include "common.h"

//#define RELEASE_DEBUG

#define BUFNUM 2048
#define STRMAX 64

static char* Buf = 0;
static char* Bufp = 0;
static int* Font = 0;

void initDebugStr(){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    Buf = new char[ BUFNUM ];
    Bufp = Buf;
    Font = new int[ 96 ];
    int img = loadImage("../framework/font.png", 0);
	for( int y= 0; y < 6; y++ ){
		for(int x=0; x < 16; x++){
			Font[ y*16+x ] = divideImage(img, x*8, y*16, 8, 16);
		}
	}
#endif
}

void freeDebugStr(){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    SAFE_DELETE_ARRAY( Buf );
    SAFE_DELETE_ARRAY( Font );
#endif
}

/*
void setDebugStr( char* str, double data ){
    setDebugStr( str, (float)data );
}

void setDebugStr( char* str, float data ){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    char s[ STRMAX ];
    sprintf_s( s, STRMAX, "%s%f", str, data );
    setDebugStr( s );
#endif
}

void setDebugStr( char* str, int data ){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    char s[ STRMAX ];
    sprintf_s( s, STRMAX, "%s%d", str, data );
    setDebugStr( s );
#endif
}
*/

void setDebugStr( const char* format, ... ){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    char str[ STRMAX ];
    va_list	args;
    va_start( args, format );
    vsprintf_s( str, format, args );
    va_end( args );

    int l = strlen( str ) + 1;//\0を含めて、Bufへコピーしていく
    for( int i = 0; i < l; i++ ){
        *Bufp = str[ i ];
        Bufp++;
        WARNING( ( Bufp - Buf > BUFNUM ), "デバッグ文字、表示しすぎ！","" );
    }
#endif
}
/*
void setDebugStr( char* str ){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    int l = strlen( str ) + 1;//\0を含めて、Bufへコピーしていく
    for( int i = 0; i < l; i++ ){
        *Bufp = str[ i ];
        Bufp++;
        WARNING( ( Bufp - Buf > BUFNUM ), "デバッグ文字、表示しすぎ！","" );
    }
#endif
}
*/
void drawDebugStr( COLOR c ){
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
    int x = 0, y = 0;
    char* p = Buf;
    Bufp--;//最後の\0を指す
    while( p < Bufp ){
        if( *p ){//\0でないなら
            drawImage( Font[ *p - 32 ], x * 8.0f, y * 19.0f, c );
            x++;
        }
        else{
            x = 0;
            y++;
        }
        p++;
    }
    Bufp = Buf;//次のフレーム描画のための初期化
#endif
}

float fps(){
	static float fps;
#if defined( _DEBUG ) || defined( RELEASE_DEBUG )
	static float frame;
	static unsigned nowTime, beforeTime = timeGetTime();

	nowTime = timeGetTime();
	frame++;
	float temp = float( nowTime - beforeTime );
	if( temp >= 1000 ) {
		fps = frame * 1000 / temp;
		frame = 0;
		beforeTime = nowTime;
	}
#endif
	return fps;
}


