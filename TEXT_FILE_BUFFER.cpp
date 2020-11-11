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
    //ファイルを開く
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
    //バッファから取り出した文字列の記憶場所を確保
    String = new char[ maxStrLen ];
}

TEXT_FILE_BUFFER::~TEXT_FILE_BUFFER(){
    SAFE_DELETE_ARRAY( String );
    SAFE_DELETE_ARRAY( Buffer );
}

const char* TEXT_FILE_BUFFER::readLine(){
    const char*& bp = BufferPointer; //変数を短くする。（bpとBufferPointerは完全に同一のポインタ）
    char* sp = String;//これは普通の文字列へのポインタ
    //区切り文字を読み飛ばす
    while( unsigned(bp - Buffer) < Size && strchr( "\r\n", *bp ) ){
        bp++;
    }
    //文字列をStringへ取り出す
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