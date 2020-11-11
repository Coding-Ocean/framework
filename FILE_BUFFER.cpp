#include <stdio.h>
#include "common.h"
#include "FILE_BUFFER.h"

FILE_BUFFER::FILE_BUFFER():
Buffer( 0 ),
Size( 0 ){
}

FILE_BUFFER::FILE_BUFFER( const char* fileName ):
Buffer( 0 ),
Size( 0 ){
    setBuffer( fileName );
}

FILE_BUFFER::~FILE_BUFFER(){
    SAFE_DELETE_ARRAY( Buffer );
}

void FILE_BUFFER::setBuffer( const char* fileName ){
    char fullName[ 128 ] = "";
    strcat_s( fullName, 128, fileName );
    FILE* fp = 0;
    fopen_s( &fp, fullName, "rb");
    WARNING( !fp, "file open error", fullName );
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
}

/*
const char* FILE_BUFFER::readString(){
    const char*& bp = BufferPointer; //�ϐ�����Z������B

    //��؂蕶��[ �X�y�[�X�A\t�^�u�A\r\n���s�A�R���}�A�Z�~�R�����A\"�_�u���N�H�[�e�[�V����]��ǂݔ�΂�
    while( bp - Buffer < Size &&  strchr( " \t\r\n,;\"", *bp ) ){
        bp++;
    }

    //�������String�֎��o��
    char* sp = String;
    while( bp - Buffer < Size && !strchr( " \t\r\n,;\"", *bp ) ){
        *sp++ = *bp++;
    }
    *sp = '\0';

    return String;
}
*/

const char* FILE_BUFFER::readString(){
    const char*& bp = BufferPointer; //�ϐ�����Z������B

    //��؂蕶����ǂݔ�΂�
    while( bp - Buffer < Size && strchr( " \t\r\n,;\"/", *bp ) ){
        // "//"�R�����g�Ȃ�s���܂œǂݔ�΂�
        if( *bp == '/' && *( bp + 1 ) == '/' ){
            while ( bp - Buffer < Size && *bp != '\r' ){
                bp++;
            }
        }
        bp++;
    }

    //�������String�֎��o��
    char* sp = String;
    // {777}�@�݂����ȃf�[�^�������Ă�'{' '}'�������o��
    if( *bp == '{' || *bp == '}' ) {
        *sp++ = *bp++; 
    }
    else{
        while ( bp - Buffer < Size && !strchr( " \t\r\n,;\"{}", *bp ) ){
            //�R�����g���łĂ����炻���ŏI��
            if( *bp == '/'  && *( bp + 1 ) == '/' ){
                break;
            }
            //String�֑��
            *sp++ = *bp++;//������++�ł��B�O�̂��߁B
        }
    }
    
    *sp = 0;

    if( String[ 0 ] == 0 ){
        return 0;
    }
    return String;
}


const char* FILE_BUFFER::readLine(){
    const char*& bp = BufferPointer; //�ϐ���Z������
    char* sp = String;
    while ( bp - Buffer < Size && *bp != '\r' ){
        *sp++ = *bp++;
    }
    *sp = '\0';
    bp += 2;//\r\n��ǂݔ�΂�

    return String;
}

void FILE_BUFFER::readOnAssumption( char* s ){
    readString();
    if( strcmp( s, String ) != 0 ){
        char tmp1[256]="�ǂނƑz�肵��������:";
        char tmp2[256]="���ۂɁA�ǂ񂾕�����:";
        strcat_s( tmp1, 256, s );
        strcat_s( tmp2, 256, String );
        WARNING( 1, tmp1, tmp2 );
    }
}

float FILE_BUFFER::readFloat(){
    readString();
    return ( float ) atof( String );
}

int FILE_BUFFER::readInt(){
    readString();
    return atoi( String );
}

unsigned FILE_BUFFER::readUnsigned(){
    readString();
    return ( unsigned ) atoi( String );
}

void FILE_BUFFER::skipNode(){
    int cnt = 1;
    while( cnt > 0 ){
        readString();
        if( String[0] == '{' ){
            cnt++;
        }
        if( String[0] == '}' ){
            cnt--;
        }
    }
}

void FILE_BUFFER::restart(){
    BufferPointer = Buffer;
}

bool FILE_BUFFER::end(){
    return ( BufferPointer - Buffer >= Size );
}

const char* FILE_BUFFER::buffer() const{
    return Buffer;
}

int FILE_BUFFER::size(){
    return Size;
}

const char* FILE_BUFFER::string() const{
    return String;
}

const char* FILE_BUFFER::bufferPointer() const{
    return BufferPointer;
}



bool FILE_BUFFER::operator==( const char* str ){
    return strcmp( String, str ) == 0;
}

bool FILE_BUFFER::operator!=( const char* str ){
    return strcmp( String, str ) != 0;
}

bool FILE_BUFFER::operator==( char c ){
    return ( String[0] == c );
}

bool FILE_BUFFER::operator!=( char c ){
    return ( String[0] != c );
}


