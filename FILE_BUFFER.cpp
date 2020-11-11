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
    //ファイルサイズを調べる
    fseek( fp, 0L, SEEK_END );//ファイルポインタをファイルの最後まで進める
    Size = ftell( fp );//ファイルサイズを取得
    rewind( fp );//ファイルポインタを最初に戻す
    //バッファを確保して一気に読み込む
    Buffer = new char[ Size + 1 ];
    fread( Buffer, 1, Size, fp );
    Buffer[ Size ] = 0;
    fclose( fp );
    //先頭アドレスを入れておく
    BufferPointer = Buffer;
}

/*
const char* FILE_BUFFER::readString(){
    const char*& bp = BufferPointer; //変数名を短くする。

    //区切り文字[ スペース、\tタブ、\r\n改行、コンマ、セミコロン、\"ダブルクォーテーション]を読み飛ばす
    while( bp - Buffer < Size &&  strchr( " \t\r\n,;\"", *bp ) ){
        bp++;
    }

    //文字列をStringへ取り出す
    char* sp = String;
    while( bp - Buffer < Size && !strchr( " \t\r\n,;\"", *bp ) ){
        *sp++ = *bp++;
    }
    *sp = '\0';

    return String;
}
*/

const char* FILE_BUFFER::readString(){
    const char*& bp = BufferPointer; //変数名を短くする。

    //区切り文字を読み飛ばす
    while( bp - Buffer < Size && strchr( " \t\r\n,;\"/", *bp ) ){
        // "//"コメントなら行末まで読み飛ばす
        if( *bp == '/' && *( bp + 1 ) == '/' ){
            while ( bp - Buffer < Size && *bp != '\r' ){
                bp++;
            }
        }
        bp++;
    }

    //文字列をStringへ取り出す
    char* sp = String;
    // {777}　みたいなデータがあっても'{' '}'だけ取り出す
    if( *bp == '{' || *bp == '}' ) {
        *sp++ = *bp++; 
    }
    else{
        while ( bp - Buffer < Size && !strchr( " \t\r\n,;\"{}", *bp ) ){
            //コメントがでてきたらそこで終了
            if( *bp == '/'  && *( bp + 1 ) == '/' ){
                break;
            }
            //Stringへ代入
            *sp++ = *bp++;//代入後に++です。念のため。
        }
    }
    
    *sp = 0;

    if( String[ 0 ] == 0 ){
        return 0;
    }
    return String;
}


const char* FILE_BUFFER::readLine(){
    const char*& bp = BufferPointer; //変数を短くする
    char* sp = String;
    while ( bp - Buffer < Size && *bp != '\r' ){
        *sp++ = *bp++;
    }
    *sp = '\0';
    bp += 2;//\r\nを読み飛ばす

    return String;
}

void FILE_BUFFER::readOnAssumption( char* s ){
    readString();
    if( strcmp( s, String ) != 0 ){
        char tmp1[256]="読むと想定した文字列:";
        char tmp2[256]="実際に、読んだ文字列:";
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


