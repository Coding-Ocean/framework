#include <d3d11.h>
class FILE_BUFFER;

class HLSL{
public:
    HLSL();
    ~HLSL();
    void createVertexShaderAndInputLayout( 
        ID3D11Device* dev, 
        FILE_BUFFER& fb, 
        D3D11_INPUT_ELEMENT_DESC* inputElementDesc, 
        unsigned numElements
    );
    void createPixelShader( ID3D11Device* dev, FILE_BUFFER& fb );
    void set( ID3D11DeviceContext* immediateContext );
private:
    ID3D11VertexShader*         VertexShader;
    ID3D11PixelShader*          PixelShader;
    ID3D11InputLayout*          InputLayout;
};