#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

Texture2D g_Texture;
float4 g_vColor, g_vCentral;
float g_fLength;
uint g_iCount;

/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vPrePrev : TEXCOORD1;
    float4 vPrev : TEXCOORD2;
    float4 vNext : TEXCOORD3;
};

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vPrePrev : TEXCOORD1;
    float4 vPrev : TEXCOORD2;
    float4 vNext : TEXCOORD3;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vAccumulate : SV_TARGET0;
    float vRevealage : SV_TARGET1;
};




StructuredBuffer<VS_IN> Trails : register(t0);

VS_OUT VS_MAIN(uint iVertexID : SV_VertexID)
{
    VS_OUT Out = (VS_OUT) 0;    
    
    Out.vPosition = float4(Trails[iVertexID].vPosition, 1.f);    
    Out.vTexcoord = float2(float(iVertexID) / (float)g_iCount, 0.f);
    Out.vPrev = Out.vPosition;
    Out.vNext = Out.vPosition;
    
    if (1 <= iVertexID)
        Out.vPrev = float4(Trails[iVertexID - 1].vPosition, 1.f);
    
    Out.vPrePrev = Out.vPrev;
    if (2 <= iVertexID)
        Out.vPrePrev = float4(Trails[iVertexID - 2].vPosition, 1.f);
    
    if (g_iCount - 1 > iVertexID)
        Out.vNext = float4(Trails[iVertexID + 1].vPosition, 1.f);

    return Out;
}

float3 CatMullRom(float3 vP0, float3 vP1, float3 vP2, float3 vP3, float fFactor)
{
    float3 vReturn;
    
    vReturn = 0.5f * ((2.0f * vP1) + (-vP0 + vP2) * fFactor + (2.0f * vP0 - 5.0f * vP1 + 4.0f * vP2 - vP3) * fFactor * fFactor +
     (-vP0 + 3.0f * vP1 - 3.0f * vP2 + vP3) * fFactor * fFactor * fFactor);
    
    
    return vReturn;
}
[maxvertexcount(18)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[8];
    
    matrix matVP;
    float3 vDiff;
    
    matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    // TEST 
    //float3 vRight = g_WorldMatrix[0].xyz * 0.5f;
    //float3 vHeight = g_WorldMatrix[1].xyz * 0.5f;
    Out[0].vPosition = float4(CatMullRom(In[0].vPrePrev.xyz, In[0].vPrev.xyz, In[0].vPosition.xyz, In[0].vNext.xyz, 0.f), 1.f);
    vDiff = normalize(Out[0].vPosition.xyz - g_vCentral.xyz) * g_fLength;
    Out[1].vPosition = float4(Out[0].vPosition.xyz - vDiff, 1.f);
    
    Out[2].vPosition = float4(CatMullRom(In[0].vPrePrev.xyz, In[0].vPrev.xyz, In[0].vPosition.xyz, In[0].vNext.xyz, 0.33f), 1.f);
    vDiff = normalize(Out[2].vPosition.xyz - g_vCentral.xyz) * g_fLength;
    Out[3].vPosition = float4(Out[2].vPosition.xyz - vDiff, 1.f);
    
    Out[4].vPosition = float4(CatMullRom(In[0].vPrePrev.xyz, In[0].vPrev.xyz, In[0].vPosition.xyz, In[0].vNext.xyz, 0.66f), 1.f);
    vDiff = normalize(Out[4].vPosition.xyz - g_vCentral.xyz) * g_fLength;
    Out[5].vPosition = float4(Out[4].vPosition.xyz - vDiff, 1.f);

    Out[6].vPosition = float4(CatMullRom(In[0].vPrePrev.xyz, In[0].vPrev.xyz, In[0].vPosition.xyz, In[0].vNext.xyz, 1.f), 1.f);
    vDiff = normalize(Out[6].vPosition.xyz - g_vCentral.xyz) * g_fLength;
    Out[7].vPosition = float4(Out[6].vPosition.xyz - vDiff, 1.f);
    
    float fTexcoordPerIndex = (1.f / (float) g_iCount);
    
    Out[0].vTexcoord = float2(In[0].vTexcoord.x, 0.f);
    Out[1].vTexcoord = float2(Out[0].vTexcoord.x, 1.f);
    Out[2].vTexcoord = float2(Out[0].vTexcoord.x + fTexcoordPerIndex * 0.33f, 0.f);
    Out[3].vTexcoord = float2(Out[2].vTexcoord.x, 1.f);
    Out[4].vTexcoord = float2(Out[0].vTexcoord.x + fTexcoordPerIndex * 0.66f, 0.f);
    Out[5].vTexcoord = float2(Out[4].vTexcoord.x, 1.f);
    Out[6].vTexcoord = float2(Out[0].vTexcoord.x + fTexcoordPerIndex * 1.f, 0.f);
    Out[7].vTexcoord = float2(Out[6].vTexcoord.x, 1.f);
    
    
    for (int i = 0; i < 8; ++i)
    {
        Out[i].vPosition = mul(Out[i].vPosition, matVP);
        Out[i].vProjPos = Out[i].vPosition;
    }
    
    for (int j = 0; j < 3; ++j)
    {
        OutStream.Append(Out[j * 2 + 0]);
        OutStream.Append(Out[j * 2 + 1]);
        OutStream.Append(Out[j * 2 + 2]);
        OutStream.RestartStrip();

        OutStream.Append(Out[j * 2 + 2]);
        OutStream.Append(Out[j * 2 + 1]);
        OutStream.Append(Out[j * 2 + 3]);
        OutStream.RestartStrip();

    }
    
    //{
    //    Out[0].vPosition = float4(In[0].vPosition.xyz - vRight.xyz + vHeight.xyz, 1.f);
    //    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    //    Out[0].vProjPos = Out[0].vPosition;
    //    Out[0].vTexcoord = float2(In[0].vTexcoord.x + 1.f / (float) (g_iCount), 0.f);
    
    //    Out[1].vPosition = float4(In[0].vPosition.xyz + vRight.xyz + vHeight.xyz, 1.f);
    //    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    //    Out[1].vProjPos = Out[1].vPosition;
    //    Out[1].vTexcoord = float2(In[0].vTexcoord.x, 0.f);
    
    //    Out[2].vPosition = float4(In[0].vPosition.xyz + vRight.xyz - vHeight.xyz, 1.f);
    //    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    //    Out[2].vProjPos = Out[2].vPosition;
    //    Out[2].vTexcoord = float2(In[0].vTexcoord.x, 1.f);
    
    //    Out[3].vPosition = float4(In[0].vPosition.xyz - vRight.xyz - vHeight.xyz, 1.f);
    //    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    //    Out[3].vProjPos = Out[3].vPosition;
    //    Out[3].vTexcoord = float2(In[0].vTexcoord.x + 1.f / (float) (g_iCount), 1.f);
       
    //    OutStream.Append(Out[0]);
    //    OutStream.Append(Out[1]);
    //    OutStream.Append(Out[2]);
    //    OutStream.RestartStrip();

    //    OutStream.Append(Out[0]);
    //    OutStream.Append(Out[2]);
    //    OutStream.Append(Out[3]);
    //    OutStream.RestartStrip();
        
    //}
    

}




/* PixelShader */
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_Texture.Sample(LinearSampler_Clamp, float2(In.vTexcoord.x, In.vTexcoord.y));
    //float4 vColor = float4(1.f, 1.f, 1.f, 1.f);
    //vColor.a = 1.f;
    //vColor.a *= In.vTexcoord.x;
    

    float fWeight = clamp(10.f / (1e-5 + pow(In.vProjPos.w / 10.f, 3.0f) + pow(In.vProjPos / 200.f, 6.f)), 1e-2, 3e3);
    fWeight = fWeight * max(min(1.0, max(max(vColor.r, vColor.g), vColor.b) * vColor.a), vColor.a);
    
    Out.vAccumulate.rgb = vColor.rgb * vColor.a * fWeight;
    Out.vAccumulate.a = vColor.a * fWeight;
    Out.vRevealage.r = vColor.a /** clamp(log(0.6f + vColor.a), 0.25f, 0.6f)*/;
    
    
    return Out;
}



technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass DefaultPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_DEFAULT();
        ComputeShader = NULL;

    }
}