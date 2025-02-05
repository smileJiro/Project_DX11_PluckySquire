#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"


/* PS ConstBuffer */ 
cbuffer BasicPixelConstData : register(b0)
{
    Material_PS Material;       // 32
    
    int useAlbedoMap;
    int useNormalMap;
    int useAOMap;
    int useMetallicMap;         // 16
    
    int useRoughnessMap;
    int useEmissiveMap;
    int useORMHMap;
    int invertNormalMapY;       // 16
}

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
/* Bone Matrix */
float4x4 g_BoneMatrices[256];
Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ORMHTexture;

float g_fFarZ = 1000.f;
int g_iFlag = 0;

float4 g_vCamPosition;

float2 g_fStartUV;
float2 g_fEndUV;


// Vertex Shader //
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
    uint4 vBlendIndicse : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    /* 자신에게 영향을 주는 Bone Matrix를 가중치에 따라 스칼라 곱을 수행한 후, 원소끼리 덧셈연산. */
    /* w 값을 새로 구하는 이유 : 영향받는 Bone 자체가 없는 Fiona의 Sword 같은 메쉬들도 일단 제대로 렌더하기위해. (배치용 Bone) */ 
    float BlendWeightW = 1.0 - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix matBones = mul(g_BoneMatrices[In.vBlendIndicse.x], In.vBlendWeights.x) +
                      mul(g_BoneMatrices[In.vBlendIndicse.y], In.vBlendWeights.y) +
                      mul(g_BoneMatrices[In.vBlendIndicse.z], In.vBlendWeights.z) +
                      mul(g_BoneMatrices[In.vBlendIndicse.w], In.vBlendWeights.w);
    
    vector vPosition = mul(float4(In.vPosition, 1.0), matBones);
    vector vNormal = mul(float4(In.vNormal, 0.0), matBones);
    vector vTangent = mul(float4(In.vTangent, 0.0), matBones);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = vTangent;
    return Out;
}


VS_OUT VS_MAIN_RENDERTARGET_UV(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float BlendWeightW = 1.0 - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix matBones = mul(g_BoneMatrices[In.vBlendIndicse.x], In.vBlendWeights.x) +
        mul(g_BoneMatrices[In.vBlendIndicse.y], In.vBlendWeights.y) +
        mul(g_BoneMatrices[In.vBlendIndicse.z], In.vBlendWeights.z) +
        mul(g_BoneMatrices[In.vBlendIndicse.w], In.vBlendWeights.w);

    vector vPosition = mul(float4(In.vPosition, 1.0), matBones);
    vector vNormal = mul(float4(In.vNormal, 0.0), matBones);
    vector vTangent = mul(float4(In.vTangent, 0.0), matBones);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = lerp(g_fStartUV, g_fEndUV, In.vTexcoord);
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = vTangent;
    return Out;
}


// PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TEXCOORD3;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 조명에 대한 방향벡터를 뒤집은 후, 노말벡터와의 내적을 통해,
    // shade 값을 구한다. 여기에 Ambient color 역시 더한다. 
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
            
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 1.f);
    float fFlag = g_iFlag;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, fFlag);
    return Out;
}

// LightDepth 기록용 PixelShader 
struct PS_OUT_LIGHTDEPTH
{
    float vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    Out.vLightDepth = In.vProjPos.w / g_fFarZ;
    
    return Out;
}



// technique : 셰이더의 기능을 구분하고 분리하기 위한 기능. 한개 이상의 pass를 포함한다.
// pass : technique에 포함된 하위 개념으로 개별 렌더링 작업에 대한 구체적인 설정을 정의한다.
// https://www.notion.so/15-Shader-Keyword-technique11-pass-10eb1e26c8a8807aad86fb2de6738a1a // 컨트롤 클릭
technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    
    pass AlphaPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NoneCullPass // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LightDepth // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

    pass RenderTargetMappingPass // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_RENDERTARGET_UV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}