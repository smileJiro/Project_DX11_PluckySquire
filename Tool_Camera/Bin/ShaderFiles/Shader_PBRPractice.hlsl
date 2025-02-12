#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"

/* PBR Texture */
TextureCube Texture_SpecularIBL;   // 별도의 환경맵을 사용한다면, 
TextureCube Texture_IrradianceIBL; // 별도의 환경맵을 사용한다면, 

Texture2D BRDFTexture;
Texture2D Texture_Albedo;          // 물체의 기본 난반사 색상.
Texture2D Texture_Normal;          // 물체의 법선벡터(질감 표현에 사용)
Texture2D Texture_AO;              // 물체가 빛을 받지 않는 부분에 대한 환경 반사 색상
Texture2D Texture_Metalic;         // 물체가 얼마나 메탈스러운지(메탈릭이 높을 수록 주변의 색상을 흡수)
Texture2D Texture_Roughness;       // 물체의 표면 거칠기(얼마나 많은 미세표면이 눈으로 향하는 법선을 가졌는가.)

// 샘플러는 일반적인 LinearSampler와 Clamp Sampler가 필요하다.
// BRDF 맵에서 데이터를 가져올 때, Clamp를 사용한다.

static const float PI = 3.141592f;
static const float Epsilon = 0.00001f;
static const float3 FDielectric = 0.04f;      // 비금속 재질의 F0;

// Function 
float3 SchlickFresnel(float3 F0, float NdotH)
{
    // Specular F, 
    // 요즘은 GPU가 매우 빨라 좀 더 정석적인 방식으로 Fresnel을 구현해도 성능상 ㄱㅊ다
    // F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0);
    return F0 + (1.0f - F0) * pow(2.0f, (-5.55473f * NdotH - 6.98316f) * NdotH);
}

float3 DiffuseIBL(float3 Albedo, float3 NormalWorld, float3 PixelToEye, float Metalic)
{
    // 
    float3 F0 = lerp(FDielectric, Albedo, Metalic);
    float3 F = SchlickFresnel(F0, dot(NormalWorld, PixelToEye));
    float3 kd = lerp(1.0f - F, 0.0f, Metalic);
    float3 irradiance = Texture_IrradianceIBL.Sample(LinearSampler, NormalWorld).rgb;
    
    return kd * Albedo * irradiance;
}

float3 SpecularIBL(float3 Albedo, float3 NormalWorld, float3 PixelToEye, float Metalic, float Roughness)
{
    float2 SpecularBRDF = BRDFTexture.Sample(LinearSampler_Clamp, float2(dot(NormalWorld, PixelToEye), 1.0 - Roughness)).rg;
    float3 SpecularIrradiance = Texture_SpecularIBL.SampleLevel(LinearSampler, reflect(-PixelToEye, NormalWorld), Roughness * 5.0f).rgb;

    float3 F0 = lerp(FDielectric, Albedo, Metalic);
    
    return (F0 * SpecularBRDF.x + SpecularBRDF.y) * SpecularIrradiance;

}

float NdfGGX(float NdotH, float Roughness)
{
    
    return 1.0f;
}

float SchlickGGX(float NdotI, float NdotO, float Roughness)
{
    
    return 1.0f;
}



/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2; // 투영 변환 행렬까지 연산 시킨 포지션 정보를 ps 로 전달한다. >>> w 값을 위해. 
    float4 vTangent : TEXCOORD3;
};

// Rendering PipeLine : Vertex Shader // 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(Out.vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
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
    float4 vTangent : TEXCOORD3;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_DiffuseTexture.SampleLevel(LinearSampler, In.vTexcoord, 2.0f);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 1.0f/*0.f*/);
    
    float fFlag = g_iFlag;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, fFlag);
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}

/* 빛이 들어와서 맞고 튕긴 반사벡터와 이 픽셀을 바라보는 시선 벡터가 이루는 각이 180일때 최대 밝기 */
/* 하지만 우린 램버트 코사인공식을 이용가능.*/
//float4 vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
//float4 vLook = normalize(In.vWorldPos - g_vCamPosition);
//
//float fSpecular = pow(max(dot(normalize(vReflect) * -1.0, vLook), 0.0), 100);
    
//               // 디퓨즈 *  명암   + ( 빛 spec * 재질 spec) * spec 세기;
//    Out.vColor = vDiffuse *
//vShade; //+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
//    //Out.vColor = float4(0.5, 0.5, 0.5, 1.0) * vShade + (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

