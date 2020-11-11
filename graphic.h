#ifndef GRAPHIC_HD
#define GRAPHIC_HD

#include"COLOR.h"

//�O���t�B�b�N�V�X�e���������A���
void initGraphic( int baseWidth, int baseHeight, bool windowed );
void freeGraphic();
void createRenderTarget();
void setViewport();
void changeSize();

//�摜�ǂݍ��݁A���
int loadImage( const char* fileName, int defaultPathFlag=1 );
int divideImage( int idx, int x, int y, int w, int h );
void divideImage( int img, int col, int row, int w, int h, int* imgs );
void releaseImage( int idx );

//�`��
void clearTarget( const COLOR& c=COLOR(0,0,0,1) );
void present();
void drawImage( int idx, float x, float y, const COLOR& c=COLOR() );
void drawImage( int idx, float x, float y, float r, const COLOR& c=COLOR() );
void drawImage( int idx, float x, float y, float z, float r, float scale, const COLOR& c=COLOR() );
void drawCircle( float x, float y, float r, const COLOR& c=COLOR() );
void mathDrawImage( int idx, float x, float y, float r, const COLOR& c = COLOR() );
void drawGradation( const COLOR& top, const COLOR& bottom );

//�t���[���Ԋu����
extern float DeltaTime;
void setDeltaTime();

//�f�o�b�O�����̂����������\�[�X����ǂݍ��ނ��߂����ɂ������֐�
int loadImageFromResource( const char* resourceName );

//�O�p�`�`��( �R�c�̕׋��p �j
class VECTOR2;
class VECTOR3;
void drawTriangle( const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2, 
	               const COLOR& c0=COLOR(), const COLOR& c1=COLOR(), const COLOR& c2=COLOR() );
void drawLine( const VECTOR3& p0, const VECTOR3& p1, const COLOR& c=COLOR() );

//������p�֐� Ultra Basic 
void drawImage( char* name, double x, double y, double r, const COLOR& c=COLOR() );
void drawLine( double sx, double sy, double ex, double ey, const COLOR& c=COLOR() );
void mathDrawImage( char* name, double x, double y, double r=0, const COLOR& c=COLOR() );
void mathDrawLine( double sx, double sy, double ex, double ey, const COLOR& c=COLOR() );
void mathDrawAxis();

//Font�֌W
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
void createFont( int fontSetIdx, const char* string, FontId fontId, int size );//�P�̕����񂩂�t�H���g�摜������
void createFont( int fontSetIdx, char** strings, int numStrings, FontId fontId, int size );//�����̕����񂩂�t�H���g�摜������
void drawFont( int fontSetIdx, const char* string, float x, float y, const COLOR& c=COLOR() );

int loadImageFromMemory( const char* fileName, int defaultPathFlag=1 );

#endif