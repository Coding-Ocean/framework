#ifndef GRAPHIC_HD
#define GRAPHIC_HD

#include"COLOR.h"

//グラフィックシステム初期化、解放
void initGraphic( int baseWidth, int baseHeight, bool windowed );
void freeGraphic();
void createRenderTarget();
void setViewport();
void changeSize();

//画像読み込み、解放
int loadImage( const char* fileName, int defaultPathFlag=1 );
int divideImage( int idx, int x, int y, int w, int h );
void divideImage( int img, int col, int row, int w, int h, int* imgs );
void releaseImage( int idx );

//描画
void clearTarget( const COLOR& c=COLOR(0,0,0,1) );
void present();
void drawImage( int idx, float x, float y, const COLOR& c=COLOR() );
void drawImage( int idx, float x, float y, float r, const COLOR& c=COLOR() );
void drawImage( int idx, float x, float y, float z, float r, float scale, const COLOR& c=COLOR() );
void drawCircle( float x, float y, float r, const COLOR& c=COLOR() );
void mathDrawImage( int idx, float x, float y, float r, const COLOR& c = COLOR() );
void drawGradation( const COLOR& top, const COLOR& bottom );

//フレーム間隔時間
extern float DeltaTime;
void setDeltaTime();

//デバッグ文字のｂｍｐをリソースから読み込むためだけにつくった関数
int loadImageFromResource( const char* resourceName );

//三角形描画( ３Ｄの勉強用 ）
class VECTOR2;
class VECTOR3;
void drawTriangle( const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2, 
	               const COLOR& c0=COLOR(), const COLOR& c1=COLOR(), const COLOR& c2=COLOR() );
void drawLine( const VECTOR3& p0, const VECTOR3& p1, const COLOR& c=COLOR() );

//超入門用関数 Ultra Basic 
void drawImage( char* name, double x, double y, double r, const COLOR& c=COLOR() );
void drawLine( double sx, double sy, double ex, double ey, const COLOR& c=COLOR() );
void mathDrawImage( char* name, double x, double y, double r=0, const COLOR& c=COLOR() );
void mathDrawLine( double sx, double sy, double ex, double ey, const COLOR& c=COLOR() );
void mathDrawAxis();

//Font関係
enum FontId { GOTHIC_FONT, MINCHO_FONT, USER_FONT };
struct FONT_SET {
    char* string;
    FontId id;
    int size;
    FONT_SET() :string(0), id(GOTHIC_FONT), size(32) {}
};
enum { FONT_SET_1, FONT_SET_2, FONT_SET_3, FONT_SET_4, FONT_SET_5 };
void loadFont( const FONT_SET& fontSet );
void drawFont( const FONT_SET& fontSet, float x, float y, const COLOR& c=COLOR());
void releaseAllFont();
void addUserFont(const char* fontFileName, const char* fontName);
void releaseUserFont();
void createFont( int fontSetIdx, const char* string, FontId fontId, int size );//１つの文字列からフォント画像をつくる
void createFont( int fontSetIdx, char** strings, int numStrings, FontId fontId, int size );//複数の文字列からフォント画像をつくる
void drawFont( int fontSetIdx, const char* string, float x, float y, const COLOR& c=COLOR() );

int loadImageFromMemory( const char* fileName, int defaultPathFlag=1 );

#endif