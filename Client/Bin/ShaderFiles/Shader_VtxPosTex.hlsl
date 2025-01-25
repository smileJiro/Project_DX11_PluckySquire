#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

float4 g_vDiffuseColor;
/* Damage Font */
bool g_isCritical = false;
float g_fDamageFontLifeTimeRatio = 0.0f;

/* HpBar */
float g_vBarRatio;
float3 g_vHpRatio;
float2 g_vTimeData;
float g_fTimeRatio;
float g_fEffectTimeRatio;
int g_iHpCount;

/* Slide Circle */
float3 g_vSlideCircleDir;
/* Dash Stmaina */
float3 g_vCurStaminaDir;

/* UltimateBar */
float g_fUltimateRatio;

/* Button Clicked */
float g_fClickedTimeRatio = 0.0f;
float4 g_vClickedColor = float4(0.0f / 255.f, 147.f / 255.f, 194.f / 255.f, 1.0f);

Texture2D g_DiffuseTexture;
Texture2D g_HpBarTexture;
Texture2D g_StaminaTexture; // 대쉬 게이지 텍스쳐.
Texture2D g_MaskTexture; 

//SPRITE ANIMATION
float2 g_vSpriteStartUV;
float2 g_vSpriteEndUV;
float g_fPixelsPerUnrealUnit;

// Color
float4 g_vColors;


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

VS_OUT VS_SPRITEANIM(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = clamp(In.vTexcoord, g_vSpriteStartUV, g_vSpriteEndUV);


    return Out;
}
// (장치가 수행)Rendering PipeLine : Projection 변환 (W 나누기 연산 진행) // 
// (장치가 수행)Rendering PipeLine : Viewport 변환 // 
// (장치가 수행)Rendering PipeLine : Rasterization // 

// Rendering PipeLine : PixelShader //
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
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT PS_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = float4(g_vColors.x, g_vColors.y, g_vColors.z, g_vColors.w);
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT PS_UIPOINTSAMPLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}



// technique : 셰이더의 기능을 구분하고 분리하기 위한 기능. 한개 이상의 pass를 포함한다.
// pass : technique에 포함된 하위 개념으로 개별 렌더링 작업에 대한 구체적인 설정을 정의한다.
// https://www.notion.so/15-Shader-Keyword-technique11-pass-10eb1e26c8a8807aad86fb2de6738a1a // 컨트롤 클릭
technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlendPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlendZwriteNonePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ColorAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

    pass SpriteAnim
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SPRITEANIM();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_POINTSAMPLE
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UIPOINTSAMPLE();
    }

}