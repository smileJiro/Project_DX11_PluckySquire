#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

/* Damage Font */
bool g_isCritical = false;
float g_fDamageFontLifeTimeRatio = 0.0f;

/* HpBar */
float g_vBarRatio;
float3 g_vHpRatio;
float2 g_vTimeData;
int g_iHpCount;

/* Slide Circle */
float3 g_vSlideCircleDir;
/* Dash Stmaina */
float3 g_vCurStaminaDir;

/* UltimateBar */
float g_fUltimateRatio;

Texture2D g_DiffuseTexture;
Texture2D g_HpBarTexture;
Texture2D g_StaminaTexture; // 대쉬 게이지 텍스쳐.
Texture2D g_MaskTexture; 

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

/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.01f)
        discard;
    
    return Out;
}

PS_OUT PS_MAIN_GROUNDMARKER_MONSTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord) * float4(0.992f, 0.827f, 0.227f, 0.7f);

    return Out;
}


/* PixelShader */
PS_OUT PS_DAMAGEFONT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    // 만약 크리티컬이라면, 파랑 계열을 죽인다. 
    if (g_isCritical)
        Out.vColor.b *= 0.1f;

    Out.vColor.a *= saturate(1.3f - g_fDamageFontLifeTimeRatio);
    
    return Out;
}
/* PixelShader */
PS_OUT PS_HPBAR_MONSTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.rgb += saturate((float) g_iHpCount * 0.3f);
    
    float4 vHpColor = g_HpBarTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vHpRatio = g_vHpRatio;
    vHpRatio.z -= (vHpRatio.z - vHpRatio.x) * (g_vTimeData.y / g_vTimeData.x);
    if (In.vTexcoord.x < vHpRatio.z)
    {
        float4 fLerpColor = vHpColor;
        fLerpColor.gb *= 0.3f;
        fLerpColor.ar *= 0.8f;

        Out.vColor = fLerpColor;
    }
    
    if (In.vTexcoord.x < vHpRatio.x)
    {
        Out.vColor = vHpColor;
    }
    

    return Out;
}

/* PixelShader */
PS_OUT PS_HPBAR_PLAYER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor.rgb += saturate((float) g_iHpCount * 0.3f);
    
    float4 vHpColor = g_HpBarTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vHpRatio = g_vHpRatio;
    vHpRatio.z -= (vHpRatio.z - vHpRatio.x) * (g_vTimeData.y / g_vTimeData.x);
    if (In.vTexcoord.x < vHpRatio.z)
    {
        float4 fLerpColor = vHpColor;
        fLerpColor.gb *= 0.3f;
        fLerpColor.ar *= 0.8f;

        Out.vColor = fLerpColor;
    }
    
    if (In.vTexcoord.x < vHpRatio.x)
    {
        Out.vColor = vHpColor;
    }
    
    float fabsTexcoordX = abs(In.vTexcoord.x - 0.5f);
    
    if (fabsTexcoordX > 0.2f)
    {
        Out.vColor.gba -= (fabsTexcoordX - 0.2f) * 0.7f;
        Out.vColor.r -= (fabsTexcoordX - 0.2f) * 0.1f;
    }



        return Out;
}
/* PixelShader */
PS_OUT PS_FPS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor.rb *= 0.1f;
    
    return Out;
}
/* PixelShader */
PS_OUT PS_ULTIMATE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    float4 vUltimateColor = g_HpBarTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vUltimateRatio = g_vHpRatio;
    //vUltimateRatio.z -= (vUltimateRatio.z - vUltimateRatio.x) * (g_vTimeData.y / g_vTimeData.x);
    //if (In.vTexcoord.x < vUltimateRatio.z)
    //{
    //    float4 fLerpColor = vUltimateColor;
    //    fLerpColor.gb *= 0.3f;
    //    fLerpColor.ar *= 0.8f;

    //    Out.vColor = fLerpColor;
    //}

    //if (In.vTexcoord.x < vUltimateRatio.x)
    //{
    //    Out.vColor = vUltimateColor;
    //}
    //
    //if (In.vTexcoord < g_fUltimateRatio)
    //    Out.vColor.rg *= 0.2f;
    //
    //
    //float fabsTexcoordX = abs(In.vTexcoord.x - 0.5f);
    //
    //if (fabsTexcoordX > 0.2f)
    //{
    //    Out.vColor.rga -= (fabsTexcoordX - 0.2f) * 0.7f;
    //    Out.vColor.b -= (fabsTexcoordX - 0.2f) * 0.1f;
    //}
    
    
    
    float fabsTexcoordX = abs(In.vTexcoord.x - 0.5f);

    // vUltimateColor 적용
    // Step(A, B) : B >= A 일때 return 1.0f; 아니먄 0.1f
    float4 vBlue = { 0.1176f, 0.5686f, 0.87f, 1.0f };
    vUltimateColor *= vBlue;
    
    if (In.vTexcoord.x <= g_fUltimateRatio)
    {
        Out.vColor.rgb *= vBlue;
        Out.vColor.a += 0.2f;
    }
        
    
    if (In.vTexcoord.x <= vUltimateRatio.x)
    {
        Out.vColor = vUltimateColor;

        // fabsTexcoordX 기반 감소 계산
        float factor = max(fabsTexcoordX - 0.2f, 0.0f) * 0.7f;
        Out.vColor.rga -= factor;
        Out.vColor.b -= factor * (0.1f / 0.7f);
    }

    return Out;
}


PS_OUT PS_BUTTONDASH(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vStaminaColor = g_StaminaTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vStaminaColor2 = vStaminaColor;
    // 픽셀이 현재 이 방향벡터보다 왼쪽에 있다면 그리지 않는다. >>> 픽셀과 중점에 대한 벡터를 기반으로 좌측픽셀은 그리지 않는다.
    float3 vCurDir = g_vCurStaminaDir;
    float3 vCenter = float3(0.0f, 0.0f, 0.0f);
    float3 vCurTexcoord = float3(In.vTexcoord.x - 0.5f, (In.vTexcoord.y * -1.0f) + 0.5f, 0.0f);
    float3 vPixelDir = normalize(vCurTexcoord - vCenter);
    float3 vStartDir = float3(0.0f, 1.0f, 0.0f);
    
    
    // 외적의 결과 제트가 양수면 우측 다그린다. 
    // 음수면 픽셀벡터와 외적하여 음수일때 다그린다. 
    
    float3 vNormal = cross(vPixelDir, vCurDir);

    
    if (0.0f < cross(vStartDir, vCurDir).z)
    {
        if (0 < vNormal.z)
            vStaminaColor = 0.0f;
        if (vCenter.x < vCurTexcoord.x)
            vStaminaColor = vStaminaColor2;
   

    }
    else
    {
        if (0 < vNormal.z)
            vStaminaColor = 0.0f;
        
        if (0 < cross(vStartDir, vPixelDir).z)
            vStaminaColor = 0.0f;
    }

    
    Out.vColor += vStaminaColor.a;
    return Out;
}

PS_OUT PS_SLIDECIRCLE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vSlideCircleColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    float4 vSlideCircleColor2 = vSlideCircleColor;
    
    // 픽셀이 현재 이 방향벡터보다 왼쪽에 있다면 그리지 않는다. >>> 픽셀과 중점에 대한 벡터를 기반으로 좌측픽셀은 그리지 않는다.
    float3 vCurDir = g_vSlideCircleDir;
    float3 vCenter = float3(0.0f, 0.0f, 0.0f);
    float3 vCurTexcoord = float3(In.vTexcoord.x - 0.5f, (In.vTexcoord.y * -1.0f) + 0.5f, 0.0f);
    float3 vPixelDir = normalize(vCurTexcoord - vCenter);
    float3 vStartDir = float3(0.0f, 1.0f, 0.0f);
    
    
    // 외적의 결과 제트가 양수면 우측 다그린다. 
    // 음수면 픽셀벡터와 외적하여 음수일때 다그린다. 
    
    float3 vNormal = cross(vPixelDir, vCurDir);

    
    if (0.0f < cross(vStartDir, vCurDir).z)
    {
        if (0 < vNormal.z)
            vSlideCircleColor = 0.0f;
        if (vCenter.x < vCurTexcoord.x)
            vSlideCircleColor = vSlideCircleColor2;
    }
    else
    {
        if (0 < vNormal.z)
            vSlideCircleColor = 0.0f;
        
        if (0 < cross(vStartDir, vPixelDir).z)
            vSlideCircleColor = 0.0f;
    }

    vSlideCircleColor.a *= 1.5f;
    Out.vColor = vSlideCircleColor;
    return Out;
}
PS_OUT PS_ENERGYBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
   float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
   float4 vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    //vDiffuse.rgb += vMask.rgb;
    if (In.vTexcoord.x > g_vBarRatio)
    {
        vDiffuse.rgb = float3(0.7f, 0.7f, 0.7f);
        vDiffuse.a *= 0.9f;
    }
    Out.vColor = vDiffuse;
    return Out;
}

PS_OUT PS_BAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (In.vTexcoord.x > g_vHpRatio.x)
        discard;
    float vCenterPosX = 0.5f;
    vDiffuse.a = saturate(1.0f - length(vCenterPosX - In.vTexcoord.x));
    
    Out.vColor = vDiffuse;
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
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass GroundMarker_Player
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass GroundMarker_Monster
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GROUNDMARKER_MONSTER();
    }

    pass DamageFontPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DAMAGEFONT();
    }

    pass HpbarMonsterPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HPBAR_MONSTER();
    }

    pass HpbarPlayerPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HPBAR_PLAYER();
    }

    pass FPSPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FPS();
    }

    pass UltimateBarPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ULTIMATE();
    }

    pass ButtonDashPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BUTTONDASH();
    }

    pass SlideCirclePass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SLIDECIRCLE();
    }

    pass EnergyBar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ENERGYBAR();
    }

    pass Bar
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BAR();
    }
}