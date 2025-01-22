#include "Engine_Shader_Define.hlsli"
#include "Engine_Shader_Function.hlsli"

/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

float4 g_vColor = { 1.0f, 0.0f, 0.0f, 1.0f};

/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float4 vColor : COLOR;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

// Rendering PipeLine : Vertex Shader // 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vColor = In.vColor;
    return Out;
}


// Rendering PipeLine : PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vColor : COLOR;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = In.vColor;
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}