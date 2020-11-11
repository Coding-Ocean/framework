#include <stdio.h>
#include <string>
#include "common.h"
#include "TEXT_FILE_BUFFER.h"

TEXT_FILE_BUFFER::TEXT_FILE_BUFFER( const char* fileName, int usePath, int maxStrLen  ):
Buffer( 0 ),
BufferPointer( 0 ),
Size( 0 ),
String( 0 )
{
    FILE* fp = 0;
    //�t�@�C�����J��
    std::string fullPathName;
    if( usePath ){
        fullPathName = DATA_PATH;
        fullPathName += fileName;
    }
    else{
        fullPathName = fileName;
    }
    fopen_s( &fp, fullPathName.c_str(), "rb");
    WARNING( !fp, "file open error", fileName );
    //�t�@�C���T�C�Y�𒲂ׂ�
    fseek( fp, 0L, SEEK_END );//�t�@�C���|�C���^���t�@�C���̍Ō�܂Ői�߂�
    Size = ftell( fp );//�t�@�C���T�C�Y���擾
    rewind( fp );//�t�@�C���|�C���^���ŏ��ɖ߂�
    //�o�b�t�@���m�ۂ��Ĉ�C�ɓǂݍ���
    Buffer = new char[ Size + 1 ];
    fread( Buffer, 1, Size, fp );
    Buffer[ Size ] = 0;
    fclose( fp );
    //�擪�A�h���X�����Ă���
    BufferPointer = Buffer;
    //�o�b�t�@������o����������̋L���ꏊ���m��
    String = new char[ maxStrLen ];
}

TEXT_FILE_BUFFER::~TEXT_FILE_BUFFER(){
    SAFE_DELETE_ARRAY( String );
    SAFE_DELETE_ARRAY( Buffer );
}

const char* TEXT_FILE_BUFFER::readLine(){
    const char*& bp = BufferPointer; //�ϐ���Z������B�ibp��BufferPointer�͊��S�ɓ���̃|�C���^�j
    char* sp = String;//����͕��ʂ̕�����ւ̃|�C���^
    //��؂蕶����ǂݔ�΂�
    while( unsigned(bp - Buffer) < Size && strchr( "\r\n", *bp ) ){
        bp++;
    }
    //�������String�֎��o��
    while ( unsigned(bp - Buffer) < Size && !strchr( "\r\n", *bp ) ){
        *sp++ = *bp++;
    }
    *sp = 0;
    return String;
}

int TEXT_FILE_BUFFER::end(){
    return !strlen( String );
}

void TEXT_FILE_BUFFER::restart(){
    BufferPointer = Buffer;
}

const char* TEXT_FILE_BUFFER::bufferPointer(){
    return BufferPointer;
}

unsigned TEXT_FILE_BUFFER::size(){
    return Size;
}