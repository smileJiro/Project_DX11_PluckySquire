#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
TextureCube g_DiffuseTexture;

// HDRI 환경맵
TextureCube g_EnvTexture, g_SpecularTexture, g_IrradianceTexture;
Texture2D g_BRDFTexture;

cbuffer PixelConstData : register(b0)
{
    int iDrawTextureIndex = 0; // 0: Env, 1: Specular, 2: Irradiance
    float fMipLevel = 0.0f;
    float dummy1;
    float dummy2;
};

/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};

// Rendering PipeLine : Vertex Shader // 
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

VS_OUT VS_HDRI_ENV(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}


// Rendering PipeLine : PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    /* 알파값 설정도 가능 */
    //Out.vColor.a *= 0.5f;
    return Out;
}

PS_OUT PS_HDRI_ENV(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (iDrawTextureIndex == 0)
    {
        Out.vColor = g_EnvTexture.SampleLevel(LinearSampler, In.vTexcoord, fMipLevel);
    }
    else if (iDrawTextureIndex == 1)
    {
        Out.vColor = g_SpecularTexture.SampleLevel(LinearSampler, In.vTexcoord, fMipLevel);
    }
    else
    {
        Out.vColor = g_IrradianceTexture.SampleLevel(LinearSampler, In.vTexcoord, fMipLevel);
    }
    
    //Out.vColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    return Out;
}

technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_Front); /* 전면 추려내기 */
        SetDepthStencilState(DSS_None, 0); /* 깊이 버퍼 사용 x */
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff); 

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    
    pass HDRIEnvPass
    {
        SetRasterizerState(RS_Cull_None); /* 전면 추려내기 */
        SetDepthStencilState(DSS_None, 0); /* 깊이 버퍼 사용 x */
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_HDRI_ENV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HDRI_ENV();
    }

}