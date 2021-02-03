#pragma comment(lib, "winmm.lib" )
#pragma comment(lib, "imm32.lib" )
#pragma comment(lib, "d3d11.lib") 
#pragma comment(lib, "dsound.lib" )

#include <vector>
#include <map>
#include <string>
#include <mbstring.h>
#include <windows.h>
#include <d3d11.h>
#include "common.h"
#include "window.h"
#include "graphic.h"
#include "stb_image_reader.h"
#include "mat.h"
#include "hlsl.h"
#include "resourceUtil.h"
#include "BITMAP_RESOURCE.h"
#include "VECTOR2.h"
#include "VECTOR3.h"
#include "FILE_BUFFER.h"

#include "MATRIX.h"

using namespace std;

//--------------------------------------------------------------------------------------
//構造体型、グローバル変数
extern float DeltaTime = 0.0f;
extern float BaseWidth = 0;
extern float BaseHeight = 0;

struct IMAGE{
    ID3D11ShaderResourceView*  texture;
	ID3D11Buffer* vertexBuffer;
    float halfWidth;
    float halfHeight;
    int dividedImageFlag;//divideImageするときはtextureのコピーを使うので、この旗が立っているときはReleaseしない
    IMAGE() : texture( 0 ),vertexBuffer(0),dividedImageFlag(0){}
};
static vector<IMAGE>* Images = 0;

const CHAR* FontName[3]={
    "ＭＳ ゴシック",
    "ＭＳ 明朝",
    0
};
class KEY{
public:
    int fontId;
    int fontSize;
    char c[3];
    bool operator<( const KEY& key ) const {
        bool b;
        if(      this->fontId < key.fontId )
            b = true;
        else if( this->fontId > key.fontId )
            b = false;
        else if( this->fontSize < key.fontSize )
            b = true;
        else if( this->fontSize > key.fontSize )
            b = false;
        else if( strcmp( this->c, key.c ) < 0 )
            b = true;
        else
            b = false;
 
        return b;
    }
};
static map<KEY, int>* FontMap = 0;

static map<string, int>* UltraBasicImg = 0;

//シェーダに渡すデータ
struct VERTEX{//２Ｄ画像表示用頂点
    VECTOR3 pos;
    VECTOR2 tex;
	VERTEX() {}
};
struct VERTEX2{//drawTriangle用頂点
    VECTOR3 pos;
    COLOR color;
	VERTEX2() {}
};
VERTEX2 Vertices[3];



D3D_FEATURE_LEVEL	        FeatureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*               Dev = 0;
ID3D11DeviceContext*        ImmediateContext = 0;
IDXGISwapChain*             SwapChain = 0;
ID3D11RenderTargetView*     RenderTargetView = 0;
ID3D11DepthStencilView*     DepthStencilView = 0;
ID3D11DepthStencilState*    DepthStencilState = 0;
ID3D11Buffer*               CBWorld = 0;
ID3D11Buffer*               CBDiffuse;
ID3D11SamplerState*         SamplerLinear = 0;
ID3D11BlendState*			BlendState = 0;
ID3D11RasterizerState *		RasterizerState = 0;

ID3D11Buffer*               VertexBuffer2 = 0;//drawTriangle用
ID3D11Buffer*               VertexBuffer3 = 0;//drawCircle用

HLSL Hlsl[ 2 ];

UINT VsyncInterval=1;

//double HalfWidth;
//double HalfHeight;

//float Depth;

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain

void initGraphic( int baseWidth, int baseHeight, bool windowed ){

    //基準となる幅と高さ
    BaseWidth = (float)baseWidth;
    BaseHeight = (float)baseHeight;

    //イメージ配列作成
    Images = new vector<IMAGE>;

    //フォント
    FontMap = new map<KEY,int>;

    //ウルトラベーシック
    UltraBasicImg = new map<string, int>;

    HRESULT hr = S_OK;

    // Create Direct3D device and swap chain
	HWND hWnd = FindWindow(CLASS_NAME,0);

 auto hdc = GetDC(hWnd);
 auto rate = GetDeviceCaps(hdc, VREFRESH);
 if( rate >= 120 ) VsyncInterval = 2;



    UINT createDeviceFlags = 0;

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = ClientWidth;
    swapChainDesc.BufferDesc.Height = ClientHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = windowed;
    //swapChainDesc.Windowed = FALSE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ ){
        hr = D3D11CreateDeviceAndSwapChain(
			NULL, driverTypes[driverTypeIndex], NULL, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &Dev, &FeatureLevel, &ImmediateContext );
        if( SUCCEEDED( hr ) ){
            break;
        }
    }
	WARNING( FAILED( hr ),  "グラフィックデバイスが作れません", "" );

    createRenderTarget();

    // Setup the viewport
    setViewport();

	//-----------------------------------------------------------------------------------
    // Shader
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D11_INPUT_ELEMENT_DESC inputElementDesc2[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    D3D11_INPUT_ELEMENT_DESC inputElementDesc3[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

#if 1
    FILE_BUFFER fbVS( "data/shader/shaderVS.cso" );
    FILE_BUFFER fbPS( "data/shader/shaderPS.cso" );
    FILE_BUFFER fbVS2( "data/shader/shaderVS2.cso" );
    FILE_BUFFER fbPS2( "data/shader/shaderPS2.cso" );
#else
    FILE_BUFFER fbVS("data/sample/shaderVS.cso");
    FILE_BUFFER fbPS("data/sample/shaderPS.cso");
    FILE_BUFFER fbVS2("data/sample/shaderVS2.cso");
    FILE_BUFFER fbPS2("data/sample/shaderPS2.cso");
#endif
    Hlsl[ 0 ].createVertexShaderAndInputLayout( Dev, fbVS, inputElementDesc, 2 );
    Hlsl[ 0 ].createPixelShader( Dev, fbPS );
    Hlsl[ 1 ].createVertexShaderAndInputLayout( Dev, fbVS2, inputElementDesc2, 2 );
    Hlsl[ 1 ].createPixelShader( Dev, fbPS2 );

    // Set primitive topology -------------------------------------------------------------------
    ImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
    //ImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    //ImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // Vertex Buffer for drawTriangle
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.ByteWidth = sizeof(VERTEX2) * 3;
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory( &initData, sizeof(initData) );
    initData.pSysMem = Vertices;
    Dev->CreateBuffer(&bufferDesc, &initData, &VertexBuffer2);

    bufferDesc.ByteWidth = sizeof(VERTEX2) * 36;
    Dev->CreateBuffer(&bufferDesc, &initData, &VertexBuffer3);

    // Create the constant buffers 
    ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.ByteWidth = sizeof( MAT );
    hr = Dev->CreateBuffer( &bufferDesc, NULL, &CBWorld );
    WARNING( FAILED( hr ), "cannot Create CBWorld Constant Buffers", "" );
    bufferDesc.ByteWidth = sizeof(COLOR);
    hr = Dev->CreateBuffer(&bufferDesc, NULL, &CBDiffuse);
    WARNING(FAILED(hr), "cannot Create CBDiffuse Constant Buffers", "");
    ImmediateContext->VSSetConstantBuffers(0, 1, &CBWorld);
    ImmediateContext->PSSetConstantBuffers(1, 1, &CBDiffuse);

    // Create the sample state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory( &samplerDesc, sizeof(samplerDesc) );
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = Dev->CreateSamplerState( &samplerDesc, &SamplerLinear );
    WARNING( FAILED( hr ), "cannot Create sampler state", "" );
    ImmediateContext->PSSetSamplers(0, 1, &SamplerLinear);

	//ブレンディングステート生成
	D3D11_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	for(int i=0; i < 1; i++){
		blendDesc.RenderTarget[i].BlendEnable = TRUE;
		blendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
	Dev->CreateBlendState( &blendDesc, &BlendState );
    float blendFactor[4] = {D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO};
    ImmediateContext->OMSetBlendState(BlendState, blendFactor, 0xffffffff);    

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable =  FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	Dev->CreateRasterizerState( &rasterizerDesc,  &RasterizerState );
    ImmediateContext->RSSetState( RasterizerState );
}

void createRenderTarget(){
    // Create a render target view
    ID3D11Texture2D* backBuffer = NULL;
    HRESULT hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBuffer );
	WARNING( FAILED( hr ), "can't Get BackBuffer", "" );
    hr = Dev->CreateRenderTargetView( backBuffer, NULL, &RenderTargetView );
    backBuffer->Release();
	WARNING( FAILED( hr ), "can't Create RenderTargetView", "" );

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
    ZeroMemory( &depthStencilTextureDesc, sizeof(depthStencilTextureDesc) );
    depthStencilTextureDesc.Width = ClientWidth;
    depthStencilTextureDesc.Height = ClientHeight;
    depthStencilTextureDesc.MipLevels = 1;
    depthStencilTextureDesc.ArraySize = 1;
    depthStencilTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilTextureDesc.SampleDesc.Count = 1;
    depthStencilTextureDesc.SampleDesc.Quality = 0;
    depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilTextureDesc.CPUAccessFlags = 0;
    depthStencilTextureDesc.MiscFlags = 0;
    ID3D11Texture2D* depthStencilTexture = 0;
    hr = Dev->CreateTexture2D( &depthStencilTextureDesc, NULL, &depthStencilTexture );
	WARNING( FAILED( hr ), "can't Create depth stencil texture", "" );

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory( &depthStencilViewDesc, sizeof(depthStencilViewDesc) );
    depthStencilViewDesc.Format = depthStencilTextureDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    hr = Dev->CreateDepthStencilView( depthStencilTexture, &depthStencilViewDesc, &DepthStencilView );
    WARNING( FAILED( hr ), "can't Create DepthStencilView", "" );
    SAFE_RELEASE( depthStencilTexture );

    // Set RenderTarget
    ImmediateContext->OMSetRenderTargets( 1, &RenderTargetView, DepthStencilView );

    // Create depth stencil state
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory( &depthStencilDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;
    hr = Dev->CreateDepthStencilState( &depthStencilDesc, &DepthStencilState );
    WARNING(FAILED(hr), "Error", "");
    ImmediateContext->OMSetDepthStencilState( DepthStencilState, 0 );
}

void setViewport(){
    D3D11_VIEWPORT vp;
    float aspect = BaseWidth / BaseHeight;
    if( (float)ClientWidth / ClientHeight >= aspect ){
        float width = ClientHeight * aspect;
        float left = ( ClientWidth - width ) / 2.0f;
        vp.Width = width;
        vp.Height = (float)ClientHeight;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = left;
        vp.TopLeftY = 0;
        ImmediateContext->RSSetViewports( 1, &vp );
    }
    else{
        float height = ClientWidth / aspect;
        float top = ( ClientHeight - height ) / 2.0f;
        vp.Width = (float)ClientWidth;
        vp.Height = height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = top;
        ImmediateContext->RSSetViewports( 1, &vp );
    }
}

void changeSize(){
    if( ImmediateContext ){
        ID3D11RenderTargetView*	renderTargetView = NULL;
        ImmediateContext->OMSetRenderTargets( 1, &renderTargetView, NULL );// NULL, NULL の設定
        SAFE_RELEASE( RenderTargetView );
        SAFE_RELEASE( DepthStencilView );

        SwapChain->ResizeBuffers( 1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );

        createRenderTarget();
        setViewport();
    }
}

//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void freeGraphic(){
	if( Images ){
		for( unsigned i = 0; i < Images->size(); i++ ){
			SAFE_RELEASE( Images->at( i ).vertexBuffer );
            if( !Images->at( i ).dividedImageFlag ){
                SAFE_RELEASE( Images->at( i ).texture );
            }
        }
		SAFE_DELETE( Images );
	}

    SAFE_DELETE( UltraBasicImg );
    SAFE_DELETE( FontMap );
    SAFE_RELEASE( VertexBuffer2 );
	SAFE_RELEASE( BlendState );
	SAFE_RELEASE( RasterizerState );
    SAFE_RELEASE( SamplerLinear );
    SAFE_RELEASE( CBDiffuse );
    SAFE_RELEASE( CBWorld );

    SAFE_RELEASE( DepthStencilView );
    SAFE_RELEASE( RenderTargetView );
    SAFE_RELEASE( SwapChain );
    SAFE_RELEASE( ImmediateContext );
    SAFE_RELEASE( Dev );
}

void releaseImage( int i ){
	SAFE_RELEASE( Images->at( i ).vertexBuffer );
    SAFE_RELEASE( Images->at( i ).texture );
}

void clearTarget( const COLOR& c ){
	// Clear the back buffer
    float clearColor[4] = { c.r, c.g, c.b, c.a };
    ImmediateContext->ClearRenderTargetView( RenderTargetView, clearColor );
    // Clear the depth buffer to 1.0 (max depth)
    ImmediateContext->ClearDepthStencilView( DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}

void present(){
    SwapChain->Present( VsyncInterval, 0 );
}

void loadTexture( const char* pathFileName, int* texWidth, int* texHeight, ID3D11ShaderResourceView**  texture ){
    //stb_imageで画像をメモリに読み込む(アルファを強制的に追加)
	unsigned char* pixels = 0;
	int numBytePerPixel;
    pixels = stbi_load( pathFileName, texWidth, texHeight, &numBytePerPixel, 4 );
    WARNING( !pixels, "画像が読み込めない", pathFileName );
    //アルファがない場合、rgb( 0, 0, 0 )は透明化する
    if( numBytePerPixel == 3 ){
		for(int i = 0; i < *texWidth * *texHeight; i++ ){
			if( pixels[i*4]==0x00 && pixels[i*4+1]==0x00 && pixels[i*4+2]==0x00 ){
				pixels[i*4+3] = 0x00;
			}
		}
	}

    //テクスチャーとビューを創る
	D3D11_TEXTURE2D_DESC td;
	td.Width = *texWidth;
	td.Height = *texHeight;
	td.MipLevels = 1;
	td.ArraySize =1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_IMMUTABLE;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = (void*)pixels;
	sd.SysMemPitch = (UINT) (*texWidth * 4);
	sd.SysMemSlicePitch = (UINT) (*texWidth * *texHeight * 4);
    ID3D11Texture2D* pTexture=0;
	Dev->CreateTexture2D( &td, &sd, &pTexture );
	Dev->CreateShaderResourceView( pTexture, 0, texture );

    //解放
	pTexture->Release();
	stbi_image_free (pixels);
}

void createVertexBuffer(int texWidth, int texHeight, ID3D11Buffer** vertexBuffer){
    //横幅と高さから実際の頂点座標を計算
    //float cw = ( float )ClientWidth;
    float left = -texWidth / 2.0f;
    float top = texHeight / 2.0f;
    float right = texWidth / 2.0f;
    float bottom = -texHeight / 2.0f; 

	VERTEX vertices[4];
	vertices[0].pos.x=left;
	vertices[0].pos.y=top;
	vertices[0].pos.z=0.0f;
	vertices[0].tex.x=0.0f;
	vertices[0].tex.y=1.0f;
    
    vertices[1].pos.x=left;
	vertices[1].pos.y=bottom;
	vertices[1].pos.z=0.0f;
	vertices[1].tex.x=0.0f;
	vertices[1].tex.y=0.0f;

	vertices[2].pos.x=right;
	vertices[2].pos.y=top;
	vertices[2].pos.z=0.0f;
	vertices[2].tex.x=1.0f;
	vertices[2].tex.y=1.0f;

	vertices[3].pos.x=right;
	vertices[3].pos.y=bottom;
	vertices[3].pos.z=0.0f;
	vertices[3].tex.x=1.0f;
	vertices[3].tex.y=0.0f;

	D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof( VERTEX ) * 4;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory( &initData, sizeof(initData) );
    initData.pSysMem = vertices;
	Dev->CreateBuffer( &bufferDesc, &initData, vertexBuffer );
}

//テクスチャロード、IMAGEオブジェクト作成
int loadImage( const char* fileName, int defaultPathFlag ){
    
    string pathFileName;

    if( defaultPathFlag ){
        //パス＋ファイル名
        pathFileName = DATA_2DGRAPHICS_PATH;
	    pathFileName += fileName;
    }
    else{
	    pathFileName = fileName;
    }

    IMAGE img;//テンポラリ
    int texWidth, texHeight;

    loadTexture(pathFileName.c_str(), &texWidth, &texHeight, &img.texture);
    //loadTexture( fileName, &texWidth, &texHeight, &img.texture );
	createVertexBuffer( texWidth, texHeight, &img.vertexBuffer );
    img.halfWidth = texWidth / 2.0f;
    img.halfHeight = texHeight / 2.0f;

	//vectorへセット
    //空いてるところにセット
    for( unsigned i = 0; i < Images->size(); i++ ){
        if( Images->at( i ).texture == 0 ){
            Images->at( i ) = img;
            return i;
        }
    }
    //新規にセット
    Images->push_back( img );
    return ( Images->size() - 1 );
}

int loadImageFromRes(const char* resName) {
    HINSTANCE hInst = GetModuleHandle(0);
    HRSRC hRes = FindResource(hInst, resName, "IMAGE");
    WARNING(hRes == 0, "リソースがみつからない", "loadImageFromRes");
    // リソースのサイズを取得する 
    DWORD sizeOfRes = SizeofResource(hInst, hRes);
    WARNING(sizeOfRes == 0, "リソースのサイズがゼロ", "loadImageFromRes");
    // 取得したリソースをメモリにロードする
    HGLOBAL hMem = LoadResource(hInst, hRes);
    WARNING(hMem == 0, "リソースがロードできない", "loadImageFromRes");
    // ロードしたリソースデータのアドレスを取得する
    unsigned char* mem = (unsigned char*)LockResource(hMem);
    if (mem == 0) {
        FreeResource(hMem);
        WARNING(true, "リソースのアドレスが取得できない", "loadImageFromRes");
    }
    //stb_imageで読み込んでResourceViewをつくる
    unsigned char* pixels = 0;
    int texWidth = 0;
    int texHeight = 0;
    int numBytePerPixel = 0;
    pixels = stbi_load_from_memory(mem, sizeOfRes, &texWidth, &texHeight, &numBytePerPixel, 4);
    WARNING(!pixels, resName, "Load error");

    IMAGE img;//テンポラリ

    //テクスチャーとビューを創る
    D3D11_TEXTURE2D_DESC td;
    td.Width = texWidth;
    td.Height = texHeight;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_IMMUTABLE;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = (void*)pixels;
    sd.SysMemPitch = (UINT)(texWidth * 4);
    sd.SysMemSlicePitch = (UINT)(texWidth * texHeight * 4);
    ID3D11Texture2D* pTexture = 0;
    HRESULT hr;
    hr = Dev->CreateTexture2D(&td, &sd, &pTexture);
    ID3D11ShaderResourceView* obj = 0;
    hr = Dev->CreateShaderResourceView(pTexture, 0, &img.texture);
    WARNING(FAILED(hr), "resourceView", "");
    //解放
    pTexture->Release();
    stbi_image_free(pixels);
    FreeResource(hMem);

    createVertexBuffer(texWidth, texHeight, &img.vertexBuffer);
    img.halfWidth = texWidth / 2.0f;
    img.halfHeight = texHeight / 2.0f;
    
    //vectorの空いてるところにセット
    for (unsigned i = 0; i < Images->size(); i++) {
        if (Images->at(i).texture == 0) {
            Images->at(i) = img;
            return i;
        }
    }
    //新規にセット
    Images->push_back(img);
    return (Images->size() - 1);
}

//ビットマップバージョン
int loadImageFromResource( const char* resourceName ){
    BITMAP_RESOURCE bitmap( resourceName );

	D3D11_TEXTURE2D_DESC td;
	td.Width = bitmap.width();
    td.Height = bitmap.height();
	td.MipLevels = 1;
	td.ArraySize =1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_IMMUTABLE;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
    sd.pSysMem = (void*)bitmap.pixels();
	sd.SysMemPitch = bitmap.width() * 4;
	sd.SysMemSlicePitch = bitmap.width() * bitmap.height() * 4;
    IMAGE img;
	ID3D11Texture2D* pTexture;
	Dev->CreateTexture2D( &td, &sd, &pTexture );
	Dev->CreateShaderResourceView( pTexture, 0, &img.texture );
	pTexture->Release();

	createVertexBuffer( bitmap.width(), bitmap.height(), &img.vertexBuffer );
    img.halfWidth = bitmap.width() / 2.0f;
    img.halfHeight = bitmap.height() / 2.0f;

    Images->push_back( img );

    return ( Images->size() - 1 );
}

int divideImage( int idx, int x, int y, int w, int h ){
    IMAGE img;
    img.texture = Images->at( idx ).texture; 
	img.halfWidth = w / 2.0f;
	img.halfHeight= h / 2.0f;
    img.dividedImageFlag = 1; //このフラッグが立っていればtextureをリリースしない

	//img.vertexBufferを作る
	//横幅と高さから実際の頂点座標を計算
    //float cw = ( float )ClientWidth;
    float left = -w / 2.0f;
    float top = h / 2.0f;
    float right = w / 2.0f;
    float bottom = -h / 2.0f;
    float width = Images->at( idx ).halfWidth * 2.0f;
	float height = Images->at( idx ).halfHeight * 2.0f;
    VERTEX vertices[ 4 ];
    vertices[ 0 ].pos.x = left;
    vertices[ 0 ].pos.y = top;
    vertices[ 0 ].tex.x = x / width;
    vertices[ 0 ].tex.y = ( y + h ) / height;

    vertices[ 1 ].pos.x = left;
    vertices[ 1 ].pos.y = bottom;
    vertices[ 1 ].tex.x = x / width;
    vertices[ 1 ].tex.y = y / height;
    
    vertices[ 2 ].pos.x = right;
    vertices[ 2 ].pos.y = top;
    vertices[ 2 ].tex.x = ( x + w ) / width;
    vertices[ 2 ].tex.y = ( y + h ) / height;
    
    vertices[ 3 ].pos.x = right;
    vertices[ 3 ].pos.y = bottom;
    vertices[ 3 ].tex.x = ( x + w ) / width;
    vertices[ 3 ].tex.y = y / height;

    for( int i = 0; i < 4; i++ ){
        vertices[ i ].pos.z = 0.0f;
    }
    
    D3D11_BUFFER_DESC bufferDesc;
    ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof( VERTEX ) * 4;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory( &initData, sizeof( initData ) );
    initData.pSysMem = vertices;
	Dev->CreateBuffer( &bufferDesc, &initData, &img.vertexBuffer );    

	Images->push_back( img );
    //if( imageIdx ){
    //    *imageIdx = Images->size() - 1; 
    //}

    return ( Images->size() - 1 ); //imageIdxを返す
}

void divideImage( int img, int col, int row, int w, int h, int* imgs ){
    for( int y = 0; y < row; y++ ){
        for( int x = 0; x < col; x++ ){
            imgs[ x+y*col ] = divideImage( img, x*w, y*h, w, h );
        }
    }
}

void drawImage( int idx, float x, float y, const COLOR& c ){
    //プログラムを読みやすくするためのテンポラリな改名
    IMAGE& img = Images->at(idx);

    //マトリックス
    MAT mat;
    mat.ortho( 0, BaseWidth, BaseHeight, 0, -1, 1 );
	float px = x + img.halfWidth;
	float py = y + img.halfHeight;
    mat.mulTranslate( px, py, 0 );

    //描画
    Hlsl[ 0 ].set( ImmediateContext );
    ImmediateContext->UpdateSubresource( CBWorld, 0, NULL, &mat, 0, 0 );
    ImmediateContext->UpdateSubresource( CBDiffuse, 0, NULL, &c, 0, 0 );
    UINT stride = sizeof( VERTEX );
    UINT offset = 0;
    ImmediateContext->IASetVertexBuffers( 0, 1, &img.vertexBuffer, &stride, &offset );
    ImmediateContext->PSSetShaderResources( 0, 1, &img.texture );
    ImmediateContext->Draw( 4, 0 );
}

void drawImage( int idx, float x, float y,  float r, const COLOR& c ){
    //プログラムを読みやすくするためのテンポラリな改名
    IMAGE& img = Images->at(idx);

	//マトリックス
    MAT mat;
    mat.ortho( 0, BaseWidth, BaseHeight, 0, -1, 1 );
    mat.mulTranslate( x, y, 0 );
    mat.mulRotateZ( -r );

	//描画
    Hlsl[ 0 ].set( ImmediateContext );
    ImmediateContext->UpdateSubresource(CBWorld, 0, NULL, &mat, 0, 0);
    ImmediateContext->UpdateSubresource(CBDiffuse, 0, NULL, &c, 0, 0);
    UINT stride = sizeof( VERTEX );
    UINT offset = 0;
    ImmediateContext->IASetVertexBuffers( 0, 1, &img.vertexBuffer, &stride, &offset );
    ImmediateContext->PSSetShaderResources( 0, 1, &img.texture );
    ImmediateContext->Draw( 4, 0 );
}

void drawImage( int idx, float x, float y, float z, float r, float scale, const COLOR& c ){
    //プログラムを読みやすくするためのテンポラリな改名
    IMAGE& img = Images->at(idx);

	//マトリックス
    MAT mat;
    mat.ortho( 0, BaseWidth, BaseHeight, 0, -1, 1 );
    mat.mulTranslate( x, y, z );
    mat.mulRotateZ( -r );
    mat.mulScale( scale, scale, 1.0f );

	//描画
    Hlsl[ 0 ].set( ImmediateContext );
    ImmediateContext->UpdateSubresource( CBWorld, 0, NULL, &mat, 0, 0 );
    ImmediateContext->UpdateSubresource( CBDiffuse, 0, NULL, &c, 0, 0 );
    UINT stride = sizeof( VERTEX );
    UINT offset = 0;
    ImmediateContext->IASetVertexBuffers( 0, 1, &img.vertexBuffer, &stride, &offset );
    ImmediateContext->PSSetShaderResources( 0, 1, &img.texture );
    ImmediateContext->Draw( 4, 0 );
}

void setDeltaTime(){
	//unsigned preTime = timeGetTime();
	//for( int i = 0; i < 60; i++ ){
	//	clearTarget();
	//	present();
	//}
	//DeltaTime = float( ( timeGetTime() - preTime )  / 60000.0 );
	DeltaTime = 0.01666666666f;
}

void drawTriangle( const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2, const COLOR& c0, const COLOR& c1, const COLOR& c2 ){
    Vertices[0].pos = p0;
    Vertices[0].color = c0;
    Vertices[1].pos = p1;
    Vertices[1].color = c1;
    Vertices[2].pos = p2;
    Vertices[2].color = c2;

    Hlsl[ 1 ].set( ImmediateContext );
    ImmediateContext->UpdateSubresource(VertexBuffer2, 0, 0, Vertices, 0, 0);
    UINT stride = sizeof( VERTEX2 );
    UINT offset = 0;
    ImmediateContext->IASetVertexBuffers( 0, 1, &VertexBuffer2, &stride, &offset );
    ImmediateContext->Draw( 3, 0 );
}

void drawLine( const VECTOR3& p0, const VECTOR3& p1, const COLOR& c ){
    Vertices[0].pos = p0;
    Vertices[0].color = c;
    Vertices[1].pos = p1;
    Vertices[1].color = c;

    Hlsl[ 1 ].set( ImmediateContext );
    ImmediateContext->UpdateSubresource(VertexBuffer2, 0, 0, Vertices, 0, 0);
    UINT stride = sizeof( VERTEX2 );
    UINT offset = 0;
    ImmediateContext->IASetVertexBuffers( 0, 1, &VertexBuffer2, &stride, &offset );
    ImmediateContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    ImmediateContext->Draw( 2, 0 );
    ImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
}

void drawCircle( float x, float y, float r, const COLOR& c ){
    MAT mat;
    mat.ortho( 0, BaseWidth, BaseHeight, 0, -1, 1 );
    mat.mulTranslate( x, y, 0 );

    const int num=36;
    VERTEX2 CircleVertices[num];

    CircleVertices[0].pos = mat*VECTOR3( r, 0, 0 );
    CircleVertices[0].color = c;
    for( int i = 1, j = 1; i <= num/2-1; i++ ){
        float rad = 3.141592f * 2 * (i*360.0f/num) / 360.0f;
        CircleVertices[j].pos = mat * VECTOR3( cosf(-rad) * r, sinf(-rad) * r, 0 );
        CircleVertices[j++].color = c;
        CircleVertices[j].pos = mat * VECTOR3( cosf(rad) * r, sinf(rad) * r, 0 );
        CircleVertices[j++].color = c;
    }
    CircleVertices[num-1].pos = mat * VECTOR3( -r, 0, 0 );
    CircleVertices[num-1].color = c;

    Hlsl[ 1 ].set( ImmediateContext );
    ImmediateContext->UpdateSubresource( VertexBuffer3, 0, 0, CircleVertices, 0, 0 );
    UINT stride = sizeof( VERTEX2 );
    UINT offset = 0;
    ImmediateContext->IASetVertexBuffers( 0, 1, &VertexBuffer3, &stride, &offset );
    ImmediateContext->Draw( num, 0 );
}

//ultraBasic
void drawImage( char* name, double x, double y, double r, const COLOR& c ) {
    int id;
    if ( UltraBasicImg->find( name ) == UltraBasicImg->end() ) {
        id = loadImage( name );
        (*UltraBasicImg)[ name ] = id;
    }
    else {
        id = (*UltraBasicImg)[ name ];
    }
    drawImage( id, (float)x, (float)y, (float)r, c );
    //drawImage( id, (float)x, (float)y, (float)(r*0.01745328), c );
}

void drawLine( double sx, double sy, double ex, double ey, const COLOR& c) {
    VECTOR3 s;
    VECTOR3 e;
    s.x = float( sx / (BaseWidth / 2.0f) - 1 );
    e.x = float( ex / (BaseWidth / 2.0f) - 1 );
    s.y = float( 1 - sy / (BaseHeight / 2.0f) );
    e.y = float( 1 - ey / (BaseHeight  / 2.0f) );
    s.z = 0.0f;
    e.z = 0.0f;
    drawLine( s, e, c );
}

void drawGradation( const COLOR& top, const COLOR& bottom ){
    VECTOR3 lt( -1,  1, 1 );
    VECTOR3 rt(  1,  1, 1 );
    VECTOR3 lb( -1, -1, 1 );
    VECTOR3 rb(  1, -1, 1 );
    drawTriangle( lt, rt, rb, top, top, bottom );
    drawTriangle( lt, lb, rb, top, bottom, bottom );
}


//MATH系関数
void mathDrawImage( char* name, double x, double y, double r, const COLOR& c ) {
    drawImage( name, x + BaseWidth/2.0f, -y + BaseHeight/2.0f, r, c );
}

void mathDrawImage( int img, float x, float y, float r, const COLOR& c ) {
    drawImage( img, x + BaseWidth/2.0f, -y + BaseHeight/2.0f, r, c );
}

void mathDrawLine( double sx, double sy, double ex, double ey, const COLOR& c ) {
    drawLine( sx + BaseWidth/2.0, -sy + BaseHeight/2.0, ex + BaseWidth/2.0, -ey + BaseHeight/2.0, c );
}

void mathDrawAxis() {
	drawLine( VECTOR3(-1,0,0), VECTOR3(1, 0,0), COLOR( 0.5f, 0.5f, 0.5f ) );
	drawLine( VECTOR3( 0,1,0), VECTOR3(0,-1,0), COLOR( 0.5f, 0.5f, 0.5f ) );
}




// 指定文字のフォントテクスチャを取得
bool createFontTexture( char* c, int fontId, int fontsize, int* w, int* h, ID3D11ShaderResourceView**  texture ) {

   // フォントの生成
   LOGFONT lf = { fontsize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN,"" };
   strcpy_s(lf.lfFaceName,32,FontName[ fontId ] );

   HFONT hFont;
   if( !( hFont = CreateFontIndirect( &lf ) ) ) {
      return false;
   }

   // デバイスコンテキスト取得
   // デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
   HDC hdc = GetDC( NULL );
   HFONT oldFont = (HFONT)SelectObject( hdc, hFont );

   // 文字コード取得
   UINT code = 0;
//#if _UNICODE
   // unicodeの場合、文字コードは単純にワイド文字のUINT変換です
   // code = (UINT)*c;
//#else
   // マルチバイト文字の場合、
   // 1バイト文字のコードは1バイト目のUINT変換、
   // 2バイト文字のコードは[先導コード]*256 + [文字コード]です
   if(IsDBCSLeadByte(*c))
      code = (BYTE)c[0]<<8 | (BYTE)c[1];
   else
      code = c[0];
//#endif

   // フォントビットマップ取得
   TEXTMETRIC TM;
   GetTextMetrics( hdc, &TM );
   GLYPHMETRICS GM;
   CONST MAT2 Mat = { {0,1}, {0,0}, {0,0}, {0,1} };
   DWORD size = GetGlyphOutline( hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat );
   BYTE* ptr = new BYTE[ size ];
   GetGlyphOutline( hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat );

   // デバイスコンテキストとフォントハンドルの開放
   SelectObject( hdc, oldFont );
   DeleteObject( hFont );
   ReleaseDC( NULL, hdc );


   //--------------------------------
   // 書き込み可能テクスチャ作成
   //--------------------------------

   // CPUで書き込みができるテクスチャを作成します。
   // 自前でテクスチャに色をつけたい場合に使えます。

   // テクスチャ作成
   D3D11_TEXTURE2D_DESC desc;
   memset( &desc, 0, sizeof( desc ) );
   desc.Width			= GM.gmCellIncX;
   desc.Height			= TM.tmHeight;
   desc.MipLevels		= 1;
   desc.ArraySize		= 1;
   desc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(255,255,255,255)タイプ
   desc.SampleDesc.Count= 1;
   desc.Usage			= D3D11_USAGE_DYNAMIC;			// 動的（書き込みするための必須条件）
   desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;	// シェーダリソースとして使う
   desc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;	// CPUからアクセスして書き込みOK

   ID3D11Texture2D* pTexture;
   Dev->CreateTexture2D( &desc, 0, &pTexture );

   *w = desc.Width;
   *h = desc.Height;

   // テクスチャに書き込み
   // テクスチャをマップ（＝ロック）すると、
   // メモリにアクセスするための情報がD3D11_MAPPED_TEXTURE2Dに格納されます。

    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = ImmediateContext->Map( 
	    pTexture,
	    0,
	    D3D11_MAP_WRITE_DISCARD,
	    0,
	    &mapped );

    // ここで書き込む
    BYTE* pBits = (BYTE*)mapped.pData;

    // フォント情報の書き込み
    // iOfs_x, iOfs_y : 書き出し位置(左上)
    // iBmp_w, iBmp_h : フォントビットマップの幅高
    // Level : α値の段階 (GGO_GRAY4_BITMAPなので17段階)
    int iOfs_x = GM.gmptGlyphOrigin.x;
    int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
    int iBmp_w = GM.gmBlackBoxX + ( 4 - ( GM.gmBlackBoxX % 4 ) ) % 4 ;
    int iBmp_h = GM.gmBlackBoxY;
    int Level = 17;
    int x, y;
    DWORD Alpha, Color;
    memset( pBits, 0, mapped.RowPitch * TM.tmHeight );
    for( y = iOfs_y; y < iOfs_y + iBmp_h; y++ ){
        for( x = iOfs_x; x < iOfs_x + iBmp_w; x++) {
            Alpha = ( 255 * ptr[ x - iOfs_x + iBmp_w * ( y - iOfs_y ) ] ) / ( Level - 1 );
            if( code == 0x20 || code == 0x8140 )//半角全角スペースは左下に線が入るので、Colorを0にする
                Color = 0x0;
            else
                Color = 0x00ffffff | ( Alpha << 24 );
            memcpy( (BYTE*)pBits + mapped.RowPitch * y + 4 * x, &Color, sizeof(DWORD) );
        }
    }
    ImmediateContext->Unmap( pTexture, 0 );

    Dev->CreateShaderResourceView( pTexture, 0, texture );
	pTexture->Release();

    delete[] ptr;

    return true;
}

//指定文字（１文字）の描画可能データを作る
int loadFontImage( char* c, int fontId, int fontSize ){

    IMAGE img;//テンポラリ
    int texWidth, texHeight;

    createFontTexture( c, fontId, fontSize, &texWidth, &texHeight, &img.texture );
	createVertexBuffer( texWidth, texHeight, &img.vertexBuffer );
    img.halfWidth = texWidth / 2.0f;
    img.halfHeight = texHeight / 2.0f;

	//vector Imagesへセット
    Images->push_back( img );

    return ( Images->size() - 1 );
}

void loadFont( const FONT_SET& fontSet ){
    const char* p = fontSet.string;
    while( *p != 0 ){
        KEY key;
        key.fontId = fontSet.id;
        key.fontSize = fontSet.size;
        if( strchr( "\r\n", *p ) ){
            p++;
        }
        else{
            switch( _mbclen( (BYTE*)p ) ){
            case 1:
                key.c[0] = *p++;
                key.c[1] = 0;
                break;
            case 2:
                key.c[0]= *p++;
                key.c[1]= *p++;
                key.c[2]=0;
                break;
            default:
                //error
                break;
            }
            if ( FontMap->find( key ) == FontMap->end()) {
                int id = loadFontImage( key.c, fontSet.id, fontSet.size );
                (*FontMap)[ key ] = id;
            }
        }
    }
}

void drawFont( const FONT_SET& fontSet, float x, float y, const COLOR& c  ){
    const char* p = fontSet.string;
    while( *p != 0 ){
        KEY key;
        key.fontId = fontSet.id;
        key.fontSize = fontSet.size;
        switch( _mbclen( (BYTE*)p ) ){
        case 1:
            key.c[0] = *p++;
            key.c[1] = 0;
            drawImage( (*FontMap)[ key ], x, y, c );
            x += fontSet.size / 2;
            break;
        case 2:
            key.c[0]= *p++;
            key.c[1]= *p++;
            key.c[2]=0;
            drawImage( (*FontMap)[ key ], x, y, c );
            x += fontSet.size;
            break;
        default:
            //error
            break;
        }
    }
}

void releaseAllFont(){
	map<KEY, int>::iterator it = FontMap->begin();
	while( it != FontMap->end() ){
        SAFE_RELEASE( Images->at( (*it).second ).texture );
	    SAFE_RELEASE( Images->at( (*it).second ).vertexBuffer );
		++it;
	}
    FontMap->clear();
}

char FontFileName[256];
void addUserFont( const char* fontFileName, const char* fontName ){
	DESIGNVECTOR design;
	AddFontResourceEx( fontFileName, FR_PRIVATE, &design );
    strcpy_s( FontFileName, 256, fontFileName );
	int len = strlen( fontName );
    FontName[ 2 ] = new char[ len + 1 ];
    strcpy_s( (char*)FontName[2], len+1, fontName );
}

void releaseUserFont(){
	DESIGNVECTOR design;
	RemoveFontResourceEx( FontFileName, FR_PRIVATE, &design );
    SAFE_DELETE_ARRAY( FontName[2] );
}

FONT_SET FontSet[ 5 ];
void createFont( int fontSetIdx, const char* string, FontId fontId, int size ){
    FontSet[ fontSetIdx ].string = (char*)string;
    FontSet[ fontSetIdx ].id = fontId;
    FontSet[ fontSetIdx ].size = size;
    loadFont( FontSet[ fontSetIdx ] );
}
void createFont( int fontSetIdx, char** strings, int numStrings, FontId fontId, int size  ){
    int len = 1;
    for( int i = 0; i < numStrings; i++ ){
        len += strlen( strings[ i ] );
    }
    FontSet[ fontSetIdx ].string = new char[ len ];
    FontSet[ fontSetIdx ].string[0] = 0;
    for( int i = 0; i < numStrings; i++ ){
        strcat_s( FontSet[ fontSetIdx ].string, len, strings[ i ] );
    }
    FontSet[ fontSetIdx ].id = fontId;
    FontSet[ fontSetIdx ].size = size;
    loadFont( FontSet[ fontSetIdx ] );
    delete FontSet[ fontSetIdx ].string;
    FontSet[ fontSetIdx ].string = 0;
}
void drawFont( int fontSetIdx, const char* string, float x, float y, const COLOR& c ){
    const char* p = string;
    while( *p != 0 ){
        KEY key;
        key.fontId = FontSet[ fontSetIdx ].id;
        key.fontSize = FontSet[ fontSetIdx ].size;
        switch( _mbclen( (BYTE*)p ) ){
        case 1:
            key.c[0] = *p++;
            key.c[1] = 0;
            drawImage( (*FontMap)[ key ], x, y, c );
            x += FontSet[ fontSetIdx ].size / 2;
            break;
        case 2:
            key.c[0]= *p++;
            key.c[1]= *p++;
            key.c[2]=0;
            drawImage( (*FontMap)[ key ], x, y, c );
            x += FontSet[ fontSetIdx ].size;
            break;
        }
    }
}

//test
void loadTextureFromMemory( unsigned char* buffer, int size, int* texWidth, int* texHeight, ID3D11ShaderResourceView**  texture ){
    //stb_imageで画像をメモリに読み込む(アルファを強制的に追加)
	unsigned char* pixels = 0;
	int numBytePerPixel;
    pixels = stbi_load_from_memory( buffer, size, texWidth, texHeight, &numBytePerPixel, 4 );
    WARNING( !pixels, "画像が読み込めない", "mem" );
    //アルファがない場合、rgb( 0, 0, 0 )は透明化する
    if( numBytePerPixel == 3 ){
		for(int i = 0; i < *texWidth * *texHeight; i++ ){
			if( pixels[i*4]==0x00 && pixels[i*4+1]==0x00 && pixels[i*4+2]==0x00 ){
				pixels[i*4+3] = 0x00;
			}
		}
	}

    //テクスチャーとビューを創る
	D3D11_TEXTURE2D_DESC td;
	td.Width = *texWidth;
	td.Height = *texHeight;
	td.MipLevels = 1;
	td.ArraySize =1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_IMMUTABLE;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = (void*)pixels;
	sd.SysMemPitch = (UINT) (*texWidth * 4);
	sd.SysMemSlicePitch = (UINT) (*texWidth * *texHeight * 4);
    ID3D11Texture2D* pTexture=0;
	Dev->CreateTexture2D( &td, &sd, &pTexture );
	Dev->CreateShaderResourceView( pTexture, 0, texture );

    //解放
	pTexture->Release();
	stbi_image_free (pixels);
}

int loadImageFromMemory( const char* fileName, int defaultPathFlag ){
    
    string pathFileName;

    if( defaultPathFlag ){
        //パス＋ファイル名
        pathFileName = DATA_2DGRAPHICS_PATH;
	    pathFileName += fileName;
    }
    else{
	    pathFileName = fileName;
    }

    FILE* fp = 0;
    fopen_s( &fp, pathFileName.c_str(), "rb");
    WARNING( !fp, "file open error", fileName );
    //ファイルサイズを調べる
    fseek( fp, 0L, SEEK_END );//ファイルポインタをファイルの最後まで進める
    int size = ftell( fp );//ファイルサイズを取得
    rewind( fp );//ファイルポインタを最初に戻す
    //バッファを確保して一気に読み込む
    unsigned char* buffer = new unsigned char[ size ];
    fread( buffer, 1, size, fp );
    fclose( fp );

    IMAGE img;//テンポラリ
    int texWidth, texHeight;

    loadTextureFromMemory( buffer, size, &texWidth, &texHeight, &img.texture );
    delete [] buffer;
	createVertexBuffer( texWidth, texHeight, &img.vertexBuffer );
    img.halfWidth = texWidth / 2.0f;
    img.halfHeight = texHeight / 2.0f;

	//vectorへセット
    //空いてるところにセット
    for( unsigned i = 0; i < Images->size(); i++ ){
        if( Images->at( i ).texture == 0 ){
            Images->at( i ) = img;
            return i;
        }
    }
    //新規にセット
    Images->push_back( img );
    return ( Images->size() - 1 );
}
