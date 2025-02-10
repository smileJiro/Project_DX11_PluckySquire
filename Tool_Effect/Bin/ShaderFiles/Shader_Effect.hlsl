#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture, g_NormalTexture;


Texture2D g_ShaderMaterial_0, g_ShaderMaterial_1, g_ShaderMaterial_2, g_ShaderMaterial_3, g_ShaderMaterial_4;
Texture2D g_ShaderMaterial_5, g_ShaderMaterial_6, g_ShaderMaterial_7, g_ShaderMaterial_8, g_ShaderMaterial_9;
float g_fFarZ = 1000.f;
int g_iFlag = 0;


float g_fAlphaTest, g_fColorTest;

Texture2D g_AlphaTexture, g_MaskTexture, g_NoiseTexture;
float4 g_AlphaUVScale, g_MaskUVScale, g_NoiseUVScale;


// Shader Value 
float g_fTimeAcc = 0.0f;
float4 g_vColor;

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

struct PS_COLOR
{
    float4 vDiffuse : SV_TARGET0;
};




PS_COLOR PS_MAIN(PS_IN In)
{
    PS_COLOR Out = (PS_COLOR) 0;

    Out.vDiffuse = g_vColor;
    
    return Out;
}

float g_fDissolveThreshold, g_fDissolveEdgeWidth, g_fDissolveEdgeIntensity, g_fDissolveFactor;
float4 g_vDissolveColor;

PS_COLOR PS_DISSOLVE(PS_IN In)
{
    PS_COLOR Out = (PS_COLOR) 0;
    
    float fAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_AlphaUVScale.x, g_AlphaUVScale.y))
    + float2(g_AlphaUVScale.z, g_AlphaUVScale.w) * g_fTimeAcc).r;
    float fMaskAlpha = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_MaskUVScale.x, g_MaskUVScale.y))
    + float2(g_MaskUVScale.z, g_MaskUVScale.w) * g_fTimeAcc).r;
    
    fAlpha = fMaskAlpha * fAlpha;
    
    float fDissolve = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_NoiseUVScale.x, g_NoiseUVScale.y))
    + float2(g_NoiseUVScale.z, g_NoiseUVScale.w) * g_fTimeAcc).r;
    
    float fDissolveFactor = smoothstep(g_fTimeAcc * g_fDissolveFactor - g_fDissolveEdgeWidth, g_fTimeAcc * g_fDissolveFactor + g_fDissolveEdgeWidth, fDissolve); //float fDissolveFactor = step(g_fTimeAcc * g_fDissolveFactor, fDissolve);
    if (fDissolveFactor < 0.05)
        discard;
    
    float3 vColor = g_vColor.rgb;
    fAlpha = g_vColor.a * fAlpha * fDissolveFactor;
    
    if (fAlpha < g_fAlphaTest)
        discard;
    if (length(vColor) < g_fColorTest)
        discard;

       
    Out.vDiffuse.rgb = vColor;
    Out.vDiffuse.a = fAlpha;
    
    return Out;
}

struct PS_BRIGHTCOLOR
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBloom : SV_TARGET1;
};

float g_fBloomThreshold;
float3 g_fBrightness = float3(0.2126, 0.7152, 0.0722);

PS_BRIGHTCOLOR PS_BLOOM(PS_IN In)
{
    PS_BRIGHTCOLOR Out = (PS_BRIGHTCOLOR) 0;
        
    float fAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_AlphaUVScale.x, g_AlphaUVScale.y))
    + float2(g_AlphaUVScale.z, g_AlphaUVScale.w) * g_fTimeAcc).r;
    float fMaskAlpha = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_MaskUVScale.x, g_MaskUVScale.y))
    + float2(g_MaskUVScale.z, g_MaskUVScale.w) * g_fTimeAcc).r;
    
    fAlpha = fMaskAlpha * fAlpha;
    
    float3 vColor = g_vColor;
    fAlpha = g_vColor.a * fAlpha;

    float fLuminance = dot(vColor.rgb, g_fBrightness);
    float fBrightness = max(fLuminance - g_fBloomThreshold, 0.0) / (1.0 - g_fBloomThreshold);
    
    Out.vDiffuse.rgb = vColor;
    Out.vDiffuse.a = fAlpha;
    
    Out.vBloom = float4(vColor * fBrightness, fAlpha * fBrightness);
   
    if (fAlpha < g_fAlphaTest)
        discard;
    if (length(vColor) < g_fColorTest)
        discard;
    
    
    return Out;
}

PS_BRIGHTCOLOR PS_BLOOMDISSOLVE(PS_IN In)
{
    PS_BRIGHTCOLOR Out = (PS_BRIGHTCOLOR) 0;
        
    float fAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_AlphaUVScale.x, g_AlphaUVScale.y))
    + float2(g_AlphaUVScale.z, g_AlphaUVScale.w) * g_fTimeAcc).r;
    float fMaskAlpha = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_MaskUVScale.x, g_MaskUVScale.y))
    + float2(g_MaskUVScale.z, g_MaskUVScale.w) * g_fTimeAcc).r;
    
    fAlpha = fMaskAlpha * fAlpha;
    
    float fDissolve = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * (float2(g_NoiseUVScale.x, g_NoiseUVScale.y))
    + float2(g_NoiseUVScale.z, g_NoiseUVScale.w) * g_fTimeAcc).r;
    
    float fDissolveFactor = smoothstep(g_fTimeAcc * g_fDissolveFactor - g_fDissolveEdgeWidth, g_fTimeAcc * g_fDissolveFactor + g_fDissolveEdgeWidth, fDissolve); //float fDissolveFactor = step(g_fTimeAcc * g_fDissolveFactor, fDissolve);
    if (fDissolveFactor < 0.05)
        discard;
    
    float3 vColor = g_vColor;
    fAlpha = g_vColor.a * fAlpha * fDissolveFactor;

    float fLuminance = dot(vColor.rgb, g_fBrightness);
    float fBrightness = max(fLuminance - g_fBloomThreshold, 0.0) / (1.0 - g_fBloomThreshold);
    
    Out.vDiffuse.rgb = vColor;
    Out.vDiffuse.a = fAlpha;
    
    Out.vBloom = float4(vColor * fBrightness, fAlpha * fBrightness);
   
    if (fAlpha < g_fAlphaTest)
        discard;
    if (length(vColor) < g_fColorTest)
        discard;
    
    
    return Out;
}



technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
        ComputeShader = NULL;

    }

    pass Dissolve // 1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
        ComputeShader = NULL;

    }

    pass BLOOM // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOM();
        ComputeShader = NULL;

    }

    pass BLOOMDISSOLVE // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLOOMDISSOLVE();
        ComputeShader = NULL;

    }

}

/* Blended weight */
//    if (g_fTimeAcc < length(In.vTexcoord))
//        discard;
    
//float4 vMask = g_MaskTexture.Sample(PointSampler, In.vTexcoord);
    
//float3 vColor = g_vColor * vMask.rgb;
//float fAlpha = g_fAlpha * vMask.a;
    
//float fWeight = FUNC_WEIGHT3(In.vProjPos.w, 1.f);
    
//    Out.vAccumulate.rgb = vColor * fAlpha *
//fWeight;
//    Out.vAccumulate.a = fAlpha *
//fWeight;
//    Out.vRevealage = fAlpha * clamp(log(0.6f + fAlpha), 0.25f, 0.6f);
    
/* Dissolve edge*/

    //float fAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord.x * g_AlphaUVScale).r;
    //float fMaskAlpha = g_MaskTexture.Sample(LinearSampler, In.vTexcoord * g_MaskUVScale).r;
    //fAlpha = fMaskAlpha * fAlpha;
    
    //float fDissolve = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord * g_NoiseUVScale).r;
    //float fEdgeFactor = smoothstep(0.f, 0.3f, (fDissolve - g_fTimeAcc));
    //float fEdgeGlow = smoothstep(0.f, 0.3f, (fDissolve - g_fTimeAcc)) * 1.0f;
   
    //float3 vColor = lerp(float3(0.0f, 0.281f, 0.348f), g_vColor.rgb, fEdgeFactor);
    //fAlpha += fEdgeFactor + fEdgeGlow;
    //if (fAlpha < g_fAlphaTest)
    //    discard;
    //if (length(vColor) < g_fColorTest)
    //    discard;


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

