#include "common.h"
#include "hlsl.h"
#include "FILE_BUFFER.h"

HLSL::HLSL():
    VertexShader( 0 ),
    InputLayout( 0 ),
    PixelShader( 0 )
    {
}

HLSL::~HLSL(){
    SAFE_RELEASE( VertexShader );
    SAFE_RELEASE( InputLayout );
    SAFE_RELEASE( PixelShader );
}

void HLSL::createVertexShaderAndInputLayout( 
    ID3D11Device* dev,  
    FILE_BUFFER& fb, 
    D3D11_INPUT_ELEMENT_DESC* inputElementDesc, 
    unsigned numElements 
    ){
    HRESULT hr;

    // Create the vertex shader
    hr = dev->CreateVertexShader( fb.buffer(), fb.size(), NULL, &VertexShader );
    WARNING( FAILED( hr ), "cannot CreateVertexShader", "" );

    // Create the input layout
    hr = dev->CreateInputLayout( inputElementDesc, numElements,  fb.buffer(), fb.size(), &InputLayout );
    WARNING( FAILED( hr ), "cannot CreateInputLayout", "" );
}

void HLSL::createPixelShader( ID3D11Device* dev, FILE_BUFFER& fb ){
    HRESULT hr;
    // Create the pixel shader
    hr = dev->CreatePixelShader( fb.buffer(), fb.size(), NULL, &PixelShader );
    WARNING( FAILED( hr ), "cannot CreatePixelShader", "" );
}

void HLSL::set( ID3D11DeviceContext* immediateContext ){
	immediateContext->IASetInputLayout( InputLayout );
    immediateContext->VSSetShader( VertexShader, NULL, 0 );
    immediateContext->PSSetShader( PixelShader, NULL, 0 );
}