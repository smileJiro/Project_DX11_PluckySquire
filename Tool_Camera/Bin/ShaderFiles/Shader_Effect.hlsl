#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture, g_NormalTexture;
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
    float4 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TEXCOORD3;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBrightness : SV_TARGET1;
    float2 vDistortion : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    float2 vScroll = { 0.0f, 0.0f };
    vScroll = g_vScrollSpeed * g_fTimeAcc;
    vAnimateUV = In.vTexcoord + vScroll;

    float4 vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, vAnimateUV);
    vMtrlDiffuse.a;
    vMtrlDiffuse *= g_vColor_1;


    if (vMtrlDiffuse.a < 0.1f)
        discard;
    // 밝은 영역만 추출.
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    vBrightnessColor.a = g_fBloomWeights / 4.f;
    
    Out.vBrightness = vBrightnessColor;

    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}

PS_OUT PS_LUXIYA_ULTIMATE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    float fScrollY = g_fTimeAcc * g_vScrollSpeed.y;
    vAnimateUV.y = clamp(In.vTexcoord.y + fScrollY, 0.0f, 1.0f);
    //vAnimateUV.y = In.vTexcoord.y + fScroll;
    float4 vMaskDesc = g_ShaderMaterial_0.Sample(LinearSampler, vAnimateUV);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vAnimateUV);
    vMtrlDiffuse *= g_vColor_1;
    vMtrlDiffuse.a *= vMaskDesc.r;
    

    //// Distortion     
    //float2 vDistortionAnimateUV = In.vTexcoord + sin(g_fTimeAcc * 2.0f + In.vTexcoord.y * 1.0f) * 0.1f;
    //float2 vNoise1 = g_NoiseTexture1.Sample(LinearSampler, vDistortionAnimateUV).rg;
    //float fDistortionStrength = 2.0f;
    //float2 vDistortionUV = In.vTexcoord + (vNoise1.xy * fDistortionStrength);
    //float vDistortionValue = g_ShaderMaterial_1.Sample(LinearSampler, vDistortionUV).r;
    //vDistortionUV = lerp(In.vTexcoord, vDistortionUV, vDistortionValue);
    //Out.vDistortion = float2(0.0f, 0.0f);
    //if (fScroll < 1.0f)
    //{
    //    Out.vDistortion = clamp(vDistortionUV, 0.0f, 1.0f) * vMaskDesc.r;
    //}

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    // 밝은 영역만 추출. 
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);

    
    Out.vBrightness = vBrightnessColor;
    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}



PS_OUT PS_BASH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    
    float2 vScroll = g_fTimeAcc * g_vScrollSpeed;
    vAnimateUV = vAnimateUV + vScroll;


    float4 vMtrlDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 vDiffuseMask = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 vNoiseMask = float4(0.0f, 0.0f, 0.0f, 0.0f);

    //vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, In.vTexcoord);
    //vDiffuseMask = g_ShaderMaterial_0.Sample(LinearSampler, clamp(vAnimateUV, 0.f, 1.0f));
    //vNoiseMask = g_ShaderMaterial_1.Sample(LinearSampler, clamp(vAnimateUV, 0.f, 1.0f));
    //
    //vMtrlDiffuse = g_vColor_1 * vMtrlDiffuse.r * vDiffuseMask.r * vNoiseMask.r;


    //vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, In.vTexcoord);
    vDiffuseMask = g_ShaderMaterial_0.Sample(LinearSampler, clamp(vAnimateUV, 0.f, 1.0f));
    vNoiseMask = g_ShaderMaterial_1.Sample(LinearSampler, clamp(vAnimateUV, 0.f, 1.0f));

    vMtrlDiffuse *= g_vColor_1;
    vMtrlDiffuse.a *= vDiffuseMask.r * vNoiseMask.r;

        
    float fFadeAlpha = smoothstep(0.0f, 1.0f, 1.5f - g_fPlayTimeRatio);
    vMtrlDiffuse.a *= fFadeAlpha;
    
    if (vMtrlDiffuse.a < 0.05f)
        discard;

    // 밝은 영역만 추출. 
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    vBrightnessColor.a = g_fBloomWeights / 4.f;
    
    //float3 vGlowColor = float3(0.8f, 0.2f, 0.2f);
    //vBrightnessColor.rgb *= vGlowColor.rgb;



    Out.vBrightness = vBrightnessColor;
    Out.vDiffuse = vMtrlDiffuse;
    

    return Out;
}


PS_OUT PS_SCROLL_ONETIME(PS_IN In)
{
    
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    float2 vScroll = { 0.0f, 0.0f };
    vScroll = g_vScrollSpeed * g_fTimeAcc;
    vAnimateUV = In.vTexcoord + vScroll;

    float4 vMtrlDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (vAnimateUV.x < 1.0f && vAnimateUV.x > 0.0f && vAnimateUV.y < 1.0f && vAnimateUV.x > 0.0f)
        vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, vAnimateUV);
    
    vMtrlDiffuse *= g_vColor_1;


    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    // 밝은 영역만 추출.
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    vBrightnessColor.a = g_fBloomWeights / 4.f;
    
    Out.vBrightness = vBrightnessColor;

    Out.vDiffuse = vMtrlDiffuse;

    return Out;

}

PS_OUT PS_DISTORTION(PS_IN In)
{
    
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    vAnimateUV.y += 0.5f;
    float2 vScroll = g_fTimeAcc * g_vScrollSpeed;
    vAnimateUV = vAnimateUV + vScroll;


    float4 vMtrlDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 vNoiseMask = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 vDistortionMask = float4(0.0f, 0.0f, 0.0f, 0.0f);

    vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, vAnimateUV);
    vNoiseMask = g_ShaderMaterial_1.Sample(LinearSampler, vAnimateUV);
    vDistortionMask = g_ShaderMaterial_2.Sample(LinearSampler, In.vTexcoord);

    vMtrlDiffuse *= g_vColor_1;
    vMtrlDiffuse.a *= vMtrlDiffuse.r;


    // Distortion     
    float2 vDistortionAnimateUV = In.vTexcoord + sin(g_fTimeAcc * g_fWaveSpeed + In.vTexcoord.y * g_fWaveFrequency) * g_fWaveAmplitude;
    float fDistortionStrength = 1.0f;
    float2 vDistortionUV = In.vTexcoord + (vNoiseMask.xy * fDistortionStrength);
    vDistortionUV = lerp(In.vTexcoord, vDistortionUV, vDistortionMask.r);
    Out.vDistortion = float2(0.0f, 0.0f);
    Out.vDistortion = clamp(vDistortionUV, 0.0f, 1.0f) * vDistortionMask.r;
    

    //if (vMtrlDiffuse.a < 0.05f)
    //    discard;

    // 밝은 영역만 추출. 
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    vBrightnessColor.a = g_fBloomWeights / 4.f;
    
    //float3 vGlowColor = float3(0.8f, 0.2f, 0.2f);
    //vBrightnessColor.rgb *= vGlowColor.rgb;
    
    float fFadeAlpha = smoothstep(0.0f, 1.0f, 1.5f - g_fPlayTimeRatio);
    vMtrlDiffuse.a *= fFadeAlpha;


    Out.vBrightness = vBrightnessColor;
    Out.vDiffuse = vMtrlDiffuse;
    

    return Out;

}

PS_OUT PS_LUXIYA_COREWEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    float2 vScroll = g_fTimeAcc * g_vScrollSpeed;
    vAnimateUV = vAnimateUV + vScroll;


    float4 vMtrlDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 vNoiseMask = float4(0.0f, 0.0f, 0.0f, 0.0f);

    vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vNoiseMask = g_ShaderMaterial_1.Sample(LinearSampler, vAnimateUV);

    vMtrlDiffuse.a += vNoiseMask.r;
    vMtrlDiffuse *= g_vColor_1;


    //// Distortion     
    //float2 vDistortionAnimateUV = In.vTexcoord + sin(g_fTimeAcc * g_fWaveSpeed + In.vTexcoord.y * g_fWaveFrequency) * g_fWaveAmplitude;
    //float fDistortionStrength = 1.0f;
    //float2 vDistortionUV = In.vTexcoord + (vNoiseMask.xy * fDistortionStrength);
    //vDistortionUV = lerp(In.vTexcoord, vDistortionUV, vDistortionMask.r);
    //Out.vDistortion = float2(0.0f, 0.0f);
    //Out.vDistortion = clamp(vDistortionUV, 0.0f, 1.0f) * vDistortionMask.r;
    

    //if (vMtrlDiffuse.a < 0.05f)
    //    discard;

    // 밝은 영역만 추출. 
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    vBrightnessColor.a = g_fBloomWeights / 4.f;
    
    float3 vGlowColor = float3(0.8f, 0.2f, 0.2f);
    vBrightnessColor.rgb *= vGlowColor.rgb;

    Out.vBrightness = vBrightnessColor;
    Out.vDiffuse = vMtrlDiffuse;

    return Out;

}

PS_OUT PS_ROTATIONCIRCLEEFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;

    float2 vAnimateUV = In.vTexcoord;
    float2 vScroll = { 0.0f, 0.0f };
    vScroll = g_vScrollSpeed * g_fTimeAcc;
    vAnimateUV = In.vTexcoord + vScroll;

    float4 vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, In.vTexcoord);
    float4 vMaskDesc = g_ShaderMaterial_1.Sample(LinearSampler, vAnimateUV);
    vMtrlDiffuse *= g_vColor_1;
    vMtrlDiffuse.a *= vMaskDesc.r;

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    // 밝은 영역만 추출.
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    vBrightnessColor.a = g_fBloomWeights / 4.f;
    
    Out.vBrightness = vBrightnessColor;

    Out.vDiffuse = vMtrlDiffuse;

    return Out;
}


PS_OUT PS_LEE_LASER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float fFlag = g_iFlag;
    
    float2 vAnimateUV = In.vTexcoord;
    float2 vScroll = { 0.0f, 0.0f };
    vScroll = g_vScrollSpeed * g_fTimeAcc;
    vAnimateUV = In.vTexcoord + vScroll;
    
    float4 vMtrlDiffuse = g_ShaderMaterial_0.Sample(LinearSampler, vAnimateUV);
    vMtrlDiffuse.a;
    vMtrlDiffuse *= g_vColor_1;
    
    
        
        // 밝은 영역만 추출.
    float fBrightness = dot(vMtrlDiffuse.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > g_fBrightnessThreshold) ? vMtrlDiffuse : float4(0.0f, 0.0f, 0.0f, 0.0f);
    vBrightnessColor.a = g_fBloomWeights / 4.f;
        
    float3 vGlowColor = float3(0.38f, 0.7f, 1.0f);
    vBrightnessColor.rgb *= vGlowColor.rgb;
    Out.vBrightness = vBrightnessColor;
    Out.vDiffuse = vMtrlDiffuse;
    
    return Out;
}


// technique : 셰이더의 기능을 구분하고 분리하기 위한 기능. 한개 이상의 pass를 포함한다.
// pass : technique에 포함된 하위 개념으로 개별 렌더링 작업에 대한 구체적인 설정을 정의한다.
// https://www.notion.so/15-Shader-Keyword-technique11-pass-10eb1e26c8a8807aad86fb2de6738a1a // 컨트롤 클릭
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
    }

    pass BashPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BASH();
    }

    pass ScrollOneTime // 2
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SCROLL_ONETIME();
    }


    pass Distortion_Circle // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DISTORTION();
    }
    

    pass LuxiyaCoreWeapon
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LUXIYA_COREWEAPON();
    }

    pass RotationCircleEffect
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ROTATIONCIRCLEEFFECT();
    }
    
    pass LeeLaserPass // 1
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LEE_LASER();
    }



    pass LuxiyaUltimatePass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LUXIYA_ULTIMATE();
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

