#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

float4 g_vDiffuseColor;

Texture2D g_DiffuseTexture;
float3 g_vBackGroundColor;
float g_fBrightness = 1.0f;
//SPRITE ANIMATION
float2 g_vSpriteStartUV;
float2 g_vSpriteEndUV;
float2 g_vCutStartUV;
float2 g_vCutEndUV;
float g_fPixelsPerUnrealUnit;

// Color
float4 g_vColors;
float g_fInteractionRatio;
int g_iFlag = 0;
float g_fOpaque;
float g_fSprite2DFadeAlphaRatio = 1.0f;

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

struct VS_UVCUT_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vOriginTexcoord : TEXCOORD1;
    
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

VS_OUT VS_SPRITE2D(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    

    Out.vTexcoord = clamp(In.vTexcoord, g_vSpriteStartUV, g_vSpriteEndUV);
    Out.vTexcoord = clamp(Out.vTexcoord, float2(0, 0),float2(1, 1));
    return
Out;
}


VS_UVCUT_OUT VS_SPRITE2D_UVCUT(VS_IN In)
{
    VS_UVCUT_OUT Out = (VS_UVCUT_OUT) 0;
    
    Out.vOriginTexcoord = In.vTexcoord;
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    

    Out.vTexcoord = clamp(In.vTexcoord, g_vSpriteStartUV, g_vSpriteEndUV);
    Out.vTexcoord = clamp(Out.vTexcoord, float2(0, 0),float2(1, 1));
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
struct PS_UVCUT_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vOriginTexcoord : TEXCOORD1;

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
    
    float4 vDiffuse = g_DiffuseTexture.SampleLevel(LinearSampler, In.vTexcoord, 0);
    Out.vColor = vDiffuse * g_vColors;
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    Out.vColor.a *= g_fSprite2DFadeAlphaRatio;
    
    return Out;
}

PS_OUT PS_BACKGROUND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_DiffuseTexture.SampleLevel(LinearSampler, In.vTexcoord, 0);
    vColor.rgb *= g_vBackGroundColor;
    vColor.rgb += 0.25f;
    Out.vColor = vColor;
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT PS_SPRITE2D(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.SampleLevel(LinearSampler, In.vTexcoord, 0);
    
    Out.vColor.a *= g_fSprite2DFadeAlphaRatio;
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_SPRITE2D_UVCUT(PS_UVCUT_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vOriginTexcoord.x < g_vCutStartUV.x ||
        In.vOriginTexcoord.x > g_vCutEndUV.x ||
        In.vOriginTexcoord.y < g_vCutStartUV.y ||
        In.vOriginTexcoord.y > g_vCutEndUV.y)
        discard;
    Out.vColor = g_DiffuseTexture.SampleLevel(LinearSampler, In.vTexcoord, 0);
    
    Out.vColor.a *= g_fSprite2DFadeAlphaRatio;
    if (Out.vColor.a < 0.1f)
        discard;
    
    return Out;
}

PS_OUT PS_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
  
    float4 vDiffuse = g_DiffuseTexture.SampleLevel(LinearSampler, In.vTexcoord, 0);
    Out.vColor = vDiffuse * g_vColors;
       
    if (Out.vColor.a < 0.01f)
        discard;
    
    Out.vColor.a *= g_fSprite2DFadeAlphaRatio;
    
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

PS_OUT PS_UIALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.01f)
    {
        discard;
    }
    else
    {
        Out.vColor.a = g_fOpaque;
    }
    
    return Out;
}

PS_OUT PS_DIALOGUE_BG_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);
    
    if (0.99f <= Out.vColor.r && 0.99f <= Out.vColor.g && 0.99f <= Out.vColor.b)
    {
        Out.vColor.rgb = g_vColors.rgb;
    }
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;

}


PS_OUT PS_MIX_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

    if (Out.vColor.a < 0.01f || g_vColors.w < 0.2f)
        discard;
    if (g_iFlag == 1)
    {
        Out.vColor.r *= g_vColors.r;
        Out.vColor.g *= g_vColors.g;
        Out.vColor.b *= g_vColors.b;
    }
    Out.vColor.a = g_vColors.w;

    //Out.vColor = saturate(mul(Out.vColor,float4(g_vColors.x, g_vColors.y, g_vColors.z, g_vColors.w)));

    return Out;
}

PS_OUT PS_RATIO_BOTTOM_UP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vDiffuse = g_DiffuseTexture.Sample(PointSampler, In.vTexcoord);

    float fRatio = 1.0f - g_fInteractionRatio;
    vDiffuse = fRatio < In.vTexcoord.y ? vDiffuse * g_vColors : vDiffuse;

    if (0.01f > vDiffuse.a)
        discard;
    
    Out.vColor = vDiffuse;
    return Out;
}


// technique : 셰이더의 기능을 구분하고 분리하기 위한 기능. 한개 이상의 pass를 포함한다.
// pass : technique에 포함된 하위 개념으로 개별 렌더링 작업에 대한 구체적인 설정을 정의한다.
// https://www.notion.so/15-Shader-Keyword-technique11-pass-10eb1e26c8a8807aad86fb2de6738a1a // 컨트롤 클릭
technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass DefaultPass //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlendPass  // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AlphaBlendZwriteNonePass  // 2 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass ColorAlpha  // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

    pass Sprite2D  // 4
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SPRITE2D();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPRITE2D();
    }

    pass UI_POINTSAMPLE  // 5
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UIPOINTSAMPLE();
    }

    pass UI_ALPHA  // 6
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UIALPHA();
    }
    
    pass MAPOBJECT_MIXCOLOR  // 7 
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_WithDepth, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SPRITE2D();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MIX_COLOR();
    }

    pass DIALOGUE_BG_COLOR  // 8
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DIALOGUE_BG_COLOR();
    }

    pass RATIO_BOTTOM_UP // 9
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend_OnlyDiffuse, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RATIO_BOTTOM_UP();
    }

    pass BackGroundPass //10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BACKGROUND();
    }

    pass UVCutPass//11
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SPRITE2D_UVCUT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPRITE2D_UVCUT();
    }

}