#ifndef Sound_HD
#define Sound_HD

//初期化開放系
void initSound();
void freeSound();//全ての音を開放
void releaseSound( int soundIdx );//ひとつの音だけ開放

//ロード
int loadSound( const char* filename, int usePath=1 );
int loadSoundFromMemory( const char* fileName, int usePath=1 );//test中
int duplicateSound( int i );

//出力系
void playSound( int soundIdx );
void playLoopSound( int soundIdx );//繰り返し再生
void stopSound( int soundIdx );
void setVolume( int soundIdx, long volume );
void setPan( int soundIdx, int pan );//panの値域は-10000〜10000

//お勉強
//init();
//free();
//void play( char* fileName );
//void autoReleaseSound();

#endif
