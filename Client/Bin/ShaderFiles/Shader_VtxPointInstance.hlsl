#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4        g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D       g_DiffuseTexture;

vector          g_vCamPosition;
float4          g_vParticleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;              // Point Size
    
    /* Instancing Buffer Data */
    row_major float4x4 InstancingMatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
    float4 vColor : TEXCOORD1;
    float4 vTexcoord : TEXCOORD2;
};

struct VS_OUT
{
    float4 vPosition : POSITION; // SV_POSITION : 그리기전에 필요한 변환들이 끝났어! >>> z 나누기 및 래스터라이져 과정을 수행해!  없다면 >>> 할일이 남았어.
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vTexcoord : TEXCOORD1;
};

// Rendering PipeLine : Vertex Shader // 
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    
    // Scale 값을 별도로 추출하여, PSize에 곱하여 줄 것이다. 그리고 , Instancing Matrix에서는 Scale을 1로 고정시킬 것 이다. 
    float fScaleX = length(In.InstancingMatrix._11_12_13);
    float fScaleY = length(In.InstancingMatrix._21_22_23);
    //float fScaleZ = length(In.InstancingMatrix._31_32_33);
   
    matrix FinalMatrix = float4x4(  In.InstancingMatrix._11_12_13_14,
                                    In.InstancingMatrix._21_22_23_24,
                                    In.InstancingMatrix._31_32_33_34,
                                    In.InstancingMatrix._41_42_43_44);
    
    // Size에 대한 값을 psize 하나로 처리하기 위해 기존 InstancingMatrix의 Scale을 1로 밀어버리고 곱하였다. 
    vector vPostion = mul(float4(In.vPosition, 1.0f), FinalMatrix);
    // Geometry Shader에서 기하도형을 구성하기위해 Camera의 WorldPos 를 사용할 예정이다. >> 정점을 World까지만 변환 후 GeometryShader에서 도형 찍고 View, Projection 곱하자. 
    Out.vPosition = mul(vPostion, g_WorldMatrix);
    Out.vPSize = float2(In.vPSize.x * fScaleX, In.vPSize.y * fScaleY);
    Out.vLifeTime = In.vLifeTime;
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}


struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
    float4 vTexcoord : TEXCOORD1;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
    
};


//GS_MAIN(triangle GS_IN In[3])
//GS_MAIN(line GS_IN In[2])
[maxvertexcount(6)] // VertexCount를 reserve 한다고 보면 된다. 꼭 6개의 정점을전부 보내야하는 것은아니다.
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4]; /* Output으로 내보낼 타입의 구조체 4개를 선언. 즉, 4개의 정점을 구성하고, 6개의 OutStream으로 내보낸다. */
    
    /* Camera World Position을 기반으로 현재 Shader에 들어온 Vertex를 중점으로하는 4각형을 찍을 것이다. */
    /* 주의 : 사각형 기준으로 정점을 찍지만, 카메라가 바라보는 방향기준으로 시계방향으로 정점을 찍어야한다. */
    vector vLookDir = g_vCamPosition - In[0].vPosition;
    float3 vRightDir = normalize(cross(float3(0.0f, 1.0f, 0.0f), vLookDir.xyz));
    float3 vUpDir = normalize(cross(vLookDir.xyz, vRightDir));
    
    float3 vCenter = In[0].vPosition.xyz;
    float3 vRightDist = vRightDir * In[0].vPSize.x * 0.5f;
    float3 vUpDist = vUpDir * In[0].vPSize.y * 0.5f;
    
    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    // 정점 기준 우상단 (카메라가 바라보는 기준 좌상단.)
    Out[0].vPosition = float4(vCenter + vRightDist + vUpDist, 1.0f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(In[0].vTexcoord.x, In[0].vTexcoord.y);
    Out[0].vLifeTime = In[0].vLifeTime;

    // 정점 기준 좌상단 (카메라가 바라보는 기준 우상단.)
    Out[1].vPosition = float4(vCenter - vRightDist + vUpDist, 1.0f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(In[0].vTexcoord.z, In[0].vTexcoord.y);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    // 정점 기준 좌하단 (카메라가 바라보는 기준 우하단.)
    Out[2].vPosition = float4(vCenter - vRightDist - vUpDist, 1.0f);
    Out[2].vPosition = mul(Out[2].vPosition,  matVP);
    Out[2].vTexcoord = float2(In[0].vTexcoord.z, In[0].vTexcoord.w);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    // 정점 기준 우하단 (카메라가 바라보는 기준 좌하단.)
    Out[3].vPosition = float4(vCenter + vRightDist - vUpDist, 1.0f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(In[0].vTexcoord.x, In[0].vTexcoord.w);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    
    // 만들어낸 정점들로 2개의 삼각형을 구성하자. 
    // Geometry Shader는 기본적으로 Triangle Strip을 기준으로 정점들을 구성하고 연산한다. 
    // 그래서 우린 Triangle List 처럼 사용하기 위한 꼼수가 필요하다. >>> RestartStrip() : 기존의 Strip 상태를 리셋. 처음부터 다시 정점을 구성한다.
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
}


// Rendering PipeLine : PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
    float4 vBrightness : SV_TARGET1;
};

/* PixelShader */
PS_OUT PS_MAIN_LOOP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor *= g_vParticleColor;
    if (Out.vColor.a < 0.01f)
        discard;

    // 밝은 영역만 추출.
    float fBrightness = dot(Out.vColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > 0.1f) ? Out.vColor : float4(0.0f, 0.0f, 0.0f, 0.0f);
    vBrightnessColor.a = 1.0f / 4.f;
    
    Out.vBrightness = vBrightnessColor;

    Out.vColor.a *= In.vLifeTime.y / In.vLifeTime.x;
    
    return Out;
}

PS_OUT PS_MAIN_ONCE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    Out.vColor *= g_vParticleColor;
    if (Out.vColor.a < 0.01f)
        discard;

    Out.vColor.a *= In.vLifeTime.y / In.vLifeTime.x;

    // 밝은 영역만 추출.
    float fBrightness = dot(Out.vColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 vBrightnessColor = (fBrightness > 0.1f) ? Out.vColor : float4(0.0f, 0.0f, 0.0f, 0.0f);
    vBrightnessColor.a = 1.0f / 4.f;
    
    Out.vBrightness = vBrightnessColor;
    
    if (In.vLifeTime.x <= In.vLifeTime.y)
        discard;
    
    return Out;
}

// technique : 셰이더의 기능을 구분하고 분리하기 위한 기능. 한개 이상의 pass를 포함한다.
// pass : technique에 포함된 하위 개념으로 개별 렌더링 작업에 대한 구체적인 설정을 정의한다.
// https://www.notion.so/15-Shader-Keyword-technique11-pass-10eb1e26c8a8807aad86fb2de6738a1a // 컨트롤 클릭
technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass Loop
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LOOP();
    }

    pass Once
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_ONCE();
    }

}