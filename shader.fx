Texture2D Texture : register( t0 );
SamplerState LinearSampler : register( s0 );

cbuffer b0 : register( b0 ){
    row_major matrix World;
};

cbuffer b1 : register( b1 ){
    float4 Diffuse;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT{
    float4 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct PS_INPUT{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

PS_INPUT VS( VS_INPUT i ){
    PS_INPUT o = (PS_INPUT)0;
    o.pos = mul( World, i.pos );
    o.tex = i.tex;
    return o;
}

float4 PS( PS_INPUT i) : SV_Target {
    return Texture.Sample( LinearSampler, i.tex ) * Diffuse;
}

//--------------------------------------------------------------------------------------
struct VS2_INPUT{
    float4 pos : POSITION;
    float4 color : COLOR0;
};

struct PS2_INPUT{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

PS2_INPUT VS2( VS2_INPUT i ){
    return i;
}

float4 PS2( PS2_INPUT i ) : SV_Target{
    return i.color;
}

//--------------------------------------------------------------------------------------
struct VS3_INPUT{
    float4 pos : POSITION;
};

struct PS3_INPUT{
    float4 pos : SV_POSITION;
};

PS3_INPUT VS3( VS3_INPUT i ){
    PS3_INPUT o = (PS3_INPUT)0;
    o.pos = mul( World, i.pos );
    return o;
}

float4 PS3( PS3_INPUT i) : SV_Target {
    return Diffuse;
}
