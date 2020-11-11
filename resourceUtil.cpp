#include "common.h"

unsigned getSizeOfTextResource( const char* resourceName ){
    //���\�[�X��̃V�F�[�_�[���������Ƀ��[�h����
    HINSTANCE hInst = GetModuleHandle( 0 );
    HRSRC hRes = FindResource( hInst, resourceName, "TEXT" );
    WARNING( hRes == 0, "���\�[�X���݂���Ȃ�", resourceName );
    // ���\�[�X�̃T�C�Y���擾���� 
    DWORD sizeOfRes = SizeofResource( hInst, hRes );
    WARNING( sizeOfRes == 0, "MYSHADER���\�[�X�̃T�C�Y���O","" );
    return sizeOfRes;
}

void textResourceToMem( const char* resourceName, char** memoryName ){
    //���\�[�X��̃V�F�[�_�[���������Ƀ��[�h����
    HINSTANCE hInst = GetModuleHandle( 0 );
    HRSRC hRes = FindResource( hInst, resourceName, "TEXT" );
    WARNING( hRes == 0, "���\�[�X���݂���Ȃ�", resourceName );
    // ���\�[�X�̃T�C�Y���擾���� 
    DWORD sizeOfRes = SizeofResource( hInst, hRes );
    WARNING( sizeOfRes == 0, "MYSHADER���\�[�X�̃T�C�Y���O","" );
    // �擾�������\�[�X���������Ƀ��[�h����
    HGLOBAL hMem = LoadResource( hInst, hRes );
    WARNING( hMem == 0, "MYSHADER���\�[�X�����[�h�ł��Ȃ�","" );
    // ���[�h�������\�[�X�f�[�^�̃A�h���X���擾����
    LPVOID pMem = LockResource( hMem );
    if ( pMem == 0 ){
        FreeResource( hMem );
        WARNING( 1,"���\�[�X�̃A�h���X���擾�ł��Ȃ�","" );
    }
    // ���\�[�X�̓��e�𕶎���Ƃ��Ĉ������߂̃o�b�t�@���m�ۂ���
    *memoryName = new char[ sizeOfRes + 1 ];
    // ���\�[�X�f�[�^���o�b�t�@�ɃR�s�[����
    memcpy_s( *memoryName, sizeOfRes, pMem, sizeOfRes );
    // �Ō�Ƀk������������
    *( *memoryName + sizeOfRes ) = '\0';
        //�f�o�b�O
        //::MessageBox(0,memoryName,"",0);
        //delete [] Shader;
    // ���\�[�X���������
    FreeResource( hMem );
}

