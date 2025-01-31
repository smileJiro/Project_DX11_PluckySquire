#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture, g_NormalTexture;
Texture2D g_MaskTexture;
Texture2D g_NoiseTexture;
Texture2D g_ShaderMaterial_0, g_ShaderMaterial_1, g_ShaderMaterial_2, g_ShaderMaterial_3, g_ShaderMaterial_4;
Texture2D g_ShaderMaterial_5, g_ShaderMaterial_6, g_ShaderMaterial_7, g_ShaderMaterial_8, g_ShaderMaterial_9;
float g_fFarZ = 1000.f;
int g_iFlag = 0;

// Light
vector g_vLightDir = float4(1.f, -1.f, 1.f, 0.f);
vector g_vLightDiffuse = float4(1.f, 1.f, 1.f, 1.f);
vector g_vLightAmbient = float4(1.f, 1.f, 1.f, 1.f);
vector g_vLightSpecular = float4(1.f, 1.f, 1.f, 1.f);
vector g_vMtrlAmbient = float4(0.3f, 0.3f, 0.3f, 1.f);

// Shader Value 
float g_fTimeAcc = 0.0f;
float2 g_vScrollSpeed = 0.0f;
float2 g_vScrollSpeed_Mask = 0.0f;
float g_fPlayTimeRatio = 0.0f;
float3 g_vColor;
float g_Alpha;
float4 g_vColor_1 = { 0.0f, 0.0f, 0.0f, 0.0f };
float4 g_vColor_2 = { 1.0f, 1.0f, 1.0f, 1.0f };
float4 g_vEmissiveColor = { 0.0f, 0.0f, 0.0f, 0.0f };

vector g_vCamPosition;

// Constant 
float g_fBrightnessThreshold = 0.5f;
float g_fBloomWeights = 1.0f;
float g_fWaveSpeed = 2.f;
float g_fWaveFrequency = 1.f;
float g_fWaveAmplitude = 0.1f;

// Weight에 대한 식은 총 4개
float FUNC_WEIGHT1(float fDepth, float fAlpha)
{
    return fAlpha * clamp(10.f / (1e-5 + pow(fDepth / 5.f, 2.0f) + pow(fDepth / 200.f, 6.f)), 1e-2, 3e3);
}

float FUNC_WEIGHT2(float fDepth, float fAlpha)
{
    return fAlpha * clamp(10.f / (1e-5 + pow(fDepth / 10.f, 3.0f) + pow(fDepth / 200.f, 6.f)), 1e-2, 3e3);
}

float FUNC_WEIGHT3(float fDepth, float fAlpha)
{
    return fAlpha * clamp(10.f / (1e-5 + pow(fDepth / 200.f, 4.f)), 1e-2, 3e3);
}

float FUNC_WEIGHT4(float fDepth, float fAlpha)
{
    float fdZ = (0.01f * g_fFarZ) / (fDepth - g_fFarZ) / (0.01f - g_fFarZ);
    return fAlpha * clamp(pow(1 - fdZ, 3.f), 1e-2, 3e3);
}

// Vertex Shader //
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
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
    
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(Out.vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
    return Out;

}


// PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float4 vTangent : TEXCOORD3;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBrightness : SV_TARGET1;
    float2 vDistortion : SV_TARGET2;
};

struct PS_OUT_WEIGHTEDBLENDED
{
    float4 vAccumulate : SV_TARGET0;
    float vRevealage : SV_TARGET1;
};


PS_OUT_WEIGHTEDBLENDED PS_MAIN(PS_IN In)
{
    PS_OUT_WEIGHTEDBLENDED Out = (PS_OUT_WEIGHTEDBLENDED) 0;
    
    if (g_fTimeAcc < length(In.vTexcoord))
        discard;
    
    float4 vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);
    float vNoise = g_NoiseTexture.Sample(PointSampler, In.vTexcoord).r;
    
      // 디졸브 연산
    float fDissolveAlpha = smoothstep(g_fTimeAcc - 0.1f, g_fTimeAcc, vNoise);

    // 가장자리 효과 적용
    float edgeFactor = smoothstep(g_fTimeAcc - 1.f * 0.5, g_fTimeAcc, vNoise);
    float4 edgeColor = float4(1.f, 1.f, 1.f, 1.f) * edgeFactor;

    // 최종 색상 적용
    float4 finalColor = lerp(edgeColor, float4(g_vColor, g_Alpha), fDissolveAlpha);
    finalColor.a *= fDissolveAlpha; // 알파 채널 적용
        
    float fAlpha = length(vMask.rgb) * 0.33f * g_Alpha * finalColor.a;
    
    float fWeight = FUNC_WEIGHT3(In.vProjPos.w, 1.f);
    
    Out.vAccumulate.rgb = finalColor.rgb * fAlpha * fWeight;
    Out.vAccumulate.a = fAlpha * fWeight;
    Out.vRevealage = fAlpha * clamp(log(0.6f + fAlpha), 0.25f, 0.6f);
    
    //float fAlpha = length(vMask.rgb) * 0.33f * g_Alpha;
    
    //float fWeight = FUNC_WEIGHT3(In.vProjPos.w, 1.f);
    
    //Out.vAccumulate.rgb = g_vColor * fAlpha  * fWeight;
    //Out.vAccumulate.a = fAlpha * fWeight;
    //Out.vRevealage = fAlpha * clamp(log(0.6f + fAlpha), 0.25f, 0.6f);

    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }



}


// Effect Scroll
//PS_OUT Out = (PS_OUT) 0;
//float fFlag = g_iFlag;

//float2 vAnimateUV = In.vTexcoord;
//    vAnimateUV.y = -0.5f + In.vTexcoord.y + g_fTimeAcc *
//g_fScrollSpeed;
//float4 vMtrlDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);

//    if (0.0f <= vAnimateUV.y && 1.0f >= vAnimateUV.y)
//    {
//        vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vAnimateUV);
//    }
//    vMtrlDiffuse.rgb *= g_vColor_1.
//rgb;

    
//    vMtrlDiffuse.a *= (sin(g_fPlayTimeRatio * 2 * 3.14159265358979323846) + 1.0f) * 0.5f;
    
    
//    	/* -1.f ~ 1.f -> 0.f ~ 1.f */
//float3 vNormal = { 0.0f, 0.0f, 0.0f };
//    if ((g_iFlag & FLAG_NORMALMAP) != 0)
//    {
//        // Normal map sampling 
//float3 NormalMapValue = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz;
//        vNormal = TangentToWorld_Normal(NormalMapValue.xyz, In.vNormal.xyz, In.vTangent.xyz);
       
//        // Output [-1, 1] -> [0, 1]
//        vNormal = float4(vNormal * 0.5f + 0.5f, 0.f);
//    }
//    else
//    {
//        vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
//    }

//    Out.vColor.rgb = vMtrlDiffuse.
//rgb;
//    Out.vColor.a = vMtrlDiffuse.
//a;
//    return
//Out;









// 디스토션 코드 
//if (vAnimateUV.y < g_fPlayTimeRatio)
//{
//    vMtrlDiffuse.a *= (1.0f - g_fPlayTimeRatio);
//}
    
//float2 vAnimateUV = In.vTexcoord + sin(g_fTimeAcc * 2.0f + In.vTexcoord.y * 10.0f) * 0.1f;
//float2 vNoise1 = g_NoiseTexture1.Sample(LinearSampler, vAnimateUV).rg;
//float fDistortionStrngth = 0.2f;
//float2 vDistortionUV = In.vTexcoord + (vNoise1.xy * fDistortionStrngth);
//float vDistortionValue = g_ShaderMaterial_1.Sample(LinearSampler, vDistortionUV).r;

//vDistortionUV = lerp(In.vTexcoord, vDistortionUV, vDistortionValue);
//float4 vMtrlDiffuse = 0.0f;
//vMtrlDiffuse.rgb *= g_vColor_1.rgb;

//float4 vMask1 = g_ShaderMaterial_0.Sample(LinearSampler, In.vTexcoord);

//if (vMask1.r > 0.01f)
//    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vDistortionUV);
//else
//    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

