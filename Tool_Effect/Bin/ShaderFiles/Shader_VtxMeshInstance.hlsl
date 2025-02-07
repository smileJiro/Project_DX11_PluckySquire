#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;

float g_fFarZ = 1000.f;
float g_iFlag = 0;

float4 g_vCamPosition;

/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
    
    /* Instancing Buffer Data */
    row_major float4x4 InstancingMatrix : WORLD;
    float2 vLifeTime : TEXCOORD1;
    float4 vColor : TEXCOORD2;
    float3 vVelocity : TEXCOORD3;
    float3 vForce : TEXCOORD4;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
    float2 vLifeTime : TEXCOORD3;
    float4 vColor : TEXCOORD4;
};

// Rendering PipeLine : Vertex Shader // 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matrix InstancingMatrix = float4x4(In.InstancingMatrix._11_12_13_14, In.InstancingMatrix._21_22_23_24, In.InstancingMatrix._31_32_33_34, In.InstancingMatrix._41_42_43_44);
    vector vPosition = mul(float4(In.vPosition, 1.0f), InstancingMatrix);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), InstancingMatrix * g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
 
    Out.vLifeTime = In.vLifeTime;
    Out.vColor = In.vColor;
    return Out;
}

// Rendering PipeLine : PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
    float2 vLifeTime : TEXCOORD3;
    float4 vColor : TEXCOORD4;
};

struct PS_OUT
{
    float4 vAccumulate : SV_TARGET0;
    float vRevealage : SV_TARGET1;
};

/* PixelShader */
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vMtrlDiffuse *= In.vColor;
    
    //Out.vAccumulate = vMtrlDiffuse;
    //Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
    //    
    //float fFlag = g_iFlag;
    //Out.vDepth.a = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, fFlag);
    
    float fWeight = clamp(10.f / (1e-5 + pow(In.vProjPos.w / 10.f, 3.0f) + pow(In.vProjPos.w / 200.f, 6.f)), 1e-2, 3e3)
    * max(min(1.0, max(max(vMtrlDiffuse.r, vMtrlDiffuse.g), vMtrlDiffuse.b) * vMtrlDiffuse.a), vMtrlDiffuse.a);
    
    Out.vAccumulate.rgb = vMtrlDiffuse.rgb * vMtrlDiffuse.a * fWeight;
    Out.vAccumulate.a = vMtrlDiffuse.a * fWeight;
    Out.vRevealage.r = vMtrlDiffuse.a * clamp(log(0.6f + vMtrlDiffuse.a), 0.25f, 0.6f);
    
    return Out;
}




technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    /* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass Pass_Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
        ComputeShader = NULL;

    }

}