#include "common.h"

unsigned getSizeOfTextResource( const char* resourceName ){
    //リソース上のシェーダーをメモリにロードする
    HINSTANCE hInst = GetModuleHandle( 0 );
    HRSRC hRes = FindResource( hInst, resourceName, "TEXT" );
    WARNING( hRes == 0, "リソースがみつからない", resourceName );
    // リソースのサイズを取得する 
    DWORD sizeOfRes = SizeofResource( hInst, hRes );
    WARNING( sizeOfRes == 0, "MYSHADERリソースのサイズが０","" );
    return sizeOfRes;
}

void textResourceToMem( const char* resourceName, char** memoryName ){
    //リソース上のシェーダーをメモリにロードする
    HINSTANCE hInst = GetModuleHandle( 0 );
    HRSRC hRes = FindResource( hInst, resourceName, "TEXT" );
    WARNING( hRes == 0, "リソースがみつからない", resourceName );
    // リソースのサイズを取得する 
    DWORD sizeOfRes = SizeofResource( hInst, hRes );
    WARNING( sizeOfRes == 0, "MYSHADERリソースのサイズが０","" );
    // 取得したリソースをメモリにロードする
    HGLOBAL hMem = LoadResource( hInst, hRes );
    WARNING( hMem == 0, "MYSHADERリソースがロードできない","" );
    // ロードしたリソースデータのアドレスを取得する
    LPVOID pMem = LockResource( hMem );
    if ( pMem == 0 ){
        FreeResource( hMem );
        WARNING( 1,"リソースのアドレスが取得できない","" );
    }
    // リソースの内容を文字列として扱うためのバッファを確保する
    *memoryName = new char[ sizeOfRes + 1 ];
    // リソースデータをバッファにコピーする
    memcpy_s( *memoryName, sizeOfRes, pMem, sizeOfRes );
    // 最後にヌル文字を入れる
    *( *memoryName + sizeOfRes ) = '\0';
        //デバッグ
        //::MessageBox(0,memoryName,"",0);
        //delete [] Shader;
    // リソースを解放する
    FreeResource( hMem );
}

