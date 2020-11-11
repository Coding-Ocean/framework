#include "common.h"
#include "BITMAP_RESOURCE.h"

BITMAP_RESOURCE::BITMAP_RESOURCE( const char* resourceName ):
HMem( 0 ),
Width( 0 ),
Height( 0 ),
Pixels( 0 ){

    //���\�[�X��̃V�F�[�_�[���������Ƀ��[�h����
    HINSTANCE hInst = GetModuleHandle( 0 );
    HRSRC hRes = FindResource( hInst, resourceName, "BM" );
    WARNING( hRes == 0, "���\�[�X���݂���Ȃ�", resourceName );
    // ���\�[�X�̃T�C�Y���擾���� 
    DWORD sizeOfRes = SizeofResource( hInst, hRes );
    WARNING( sizeOfRes == 0, "�r�b�g�}�b�v���\�[�X�̃T�C�Y���O","" );
    // �擾�������\�[�X���������Ƀ��[�h����
    HMem = LoadResource( hInst, hRes );
    WARNING( HMem == 0, "�r�b�g�}�b�v���\�[�X�����[�h�ł��Ȃ�", resourceName );
    // ���[�h�������\�[�X�f�[�^�̃A�h���X���擾����
    unsigned char* mem = (unsigned char*)LockResource( HMem );
    if ( mem == 0 ){
        FreeResource( HMem );
        WARNING( 1,"���\�[�X�̃A�h���X���擾�ł��Ȃ�","" );
    }

    //���ƍ������擾
    memcpy( (void*)&Width, (void*)(mem+0x12), 4);
    memcpy( (void*)&Height, (void*)(mem+0x16), 4);

    //�s�N�Z���f�[�^���擾�i�f�[�^�͋t�]���Ă���̂ň�ԃP�c�̃��C��������o���j
    Pixels = new unsigned char[ Width * Height * 4 ];
    unsigned actualWidth = Width * 3 + Width % 4;//�P���C���̃o�C�g���͂S�Ŋ���鐔�ɂȂ��Ă���
    unsigned k = 0;
	for( unsigned i = 1; i <= Height; i++){
        for( unsigned j = 0; j < Width; j++ ){
            int l = sizeOfRes - i*actualWidth + j*3;
		    Pixels[k++] = mem[l+2];
		    Pixels[k++] = mem[l+1];
		    Pixels[k++] = mem[l+0];
		    if( mem[l+0]==0x00 && mem[l+1]==0x00 && mem[l+2]==0x00){
			    Pixels[k++] = 0x00;
		    }
		    else{
			    Pixels[k++] = 0xff;
		    }
	    }
    }
}

BITMAP_RESOURCE::~BITMAP_RESOURCE(){
    // ���\�[�X���������
    FreeResource( HMem );
    SAFE_DELETE_ARRAY( Pixels );
}
