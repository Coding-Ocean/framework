#include "common.h"
#include "BITMAP_RESOURCE.h"

BITMAP_RESOURCE::BITMAP_RESOURCE( const char* resourceName ):
HMem( 0 ),
Width( 0 ),
Height( 0 ),
Pixels( 0 ){

    //リソース上のシェーダーをメモリにロードする
    HINSTANCE hInst = GetModuleHandle( 0 );
    HRSRC hRes = FindResource( hInst, resourceName, "BM" );
    WARNING( hRes == 0, "リソースがみつからない", resourceName );
    // リソースのサイズを取得する 
    DWORD sizeOfRes = SizeofResource( hInst, hRes );
    WARNING( sizeOfRes == 0, "ビットマップリソースのサイズが０","" );
    // 取得したリソースをメモリにロードする
    HMem = LoadResource( hInst, hRes );
    WARNING( HMem == 0, "ビットマップリソースがロードできない", resourceName );
    // ロードしたリソースデータのアドレスを取得する
    unsigned char* mem = (unsigned char*)LockResource( HMem );
    if ( mem == 0 ){
        FreeResource( HMem );
        WARNING( 1,"リソースのアドレスが取得できない","" );
    }

    //幅と高さを取得
    memcpy( (void*)&Width, (void*)(mem+0x12), 4);
    memcpy( (void*)&Height, (void*)(mem+0x16), 4);

    //ピクセルデータを取得（データは逆転しているので一番ケツのラインから取り出す）
    Pixels = new unsigned char[ Width * Height * 4 ];
    unsigned actualWidth = Width * 3 + Width % 4;//１ラインのバイト数は４で割れる数になっている
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
    // リソースを解放する
    FreeResource( HMem );
    SAFE_DELETE_ARRAY( Pixels );
}
