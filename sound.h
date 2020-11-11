#ifndef Sound_HD
#define Sound_HD

//�������J���n
void initSound();
void freeSound();//�S�Ẳ����J��
void releaseSound( int soundIdx );//�ЂƂ̉������J��

//���[�h
int loadSound( char* filename, int usePath=1 );
int loadSoundFromMemory( char* fileName, int usePath=1 );//test��
int duplicateSound( int i );

//�o�͌n
void playSound( int soundIdx );
void playLoopSound( int soundIdx );//�J��Ԃ��Đ�
void stopSound( int soundIdx );
void setVolume( int soundIdx, long volume );
void setPan( int soundIdx, int pan );//pan�̒l���-10000�`10000

//���׋�
//init();
//free();
//void play( char* fileName );
//void autoReleaseSound();

#endif
