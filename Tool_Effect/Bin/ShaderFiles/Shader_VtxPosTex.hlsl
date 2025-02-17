#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

Texture2D g_Texture;
float4 g_vColor;
/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_EFFECT
{
    float4 vColor : SV_TARGET0;
    float4 vBrightness : SV_TARGET1;
};


/* PixelShader */
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_vColor;    
   
    
    return Out;
}

PS_OUT PS_MAIN_Texture(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
   
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.05f)
        discard;
   
    
    return Out;
}




technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
        ComputeShader = NULL;

    }

    pass AlphaBlendTexture
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Texture();
        ComputeShader = NULL;

    }
}