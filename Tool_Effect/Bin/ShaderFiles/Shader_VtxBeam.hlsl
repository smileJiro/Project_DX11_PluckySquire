#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
/* 상수 테이블 */
float4x4 g_WorldMatrix;
float4x4 g_ViewMatrix;
float4x4 g_ProjMatrix;

Texture2D g_Texture;
float4 g_vColor;
float4 g_vLook;
float g_fWidth;

/* 구조체 */
struct VS_IN
{
    float3 vPosition : POSITION;
    float fIndex : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float fIndex : TEXCOORD0;

};

struct GS_IN
{
    float4 vPosition : POSITION;
    float fIndex : TEXCOORD0;
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
    float4 vBloom : SV_TARGET2;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matDirection;
    
    matDirection = float4x4(g_WorldMatrix._11_12_13_14, g_WorldMatrix._21_22_23_24, g_WorldMatrix._31_32_33_34, float4(0.f, 0.f, 0.f, 1.f));
        
    Out.vPosition = mul(float4(In.vPosition, 1.f), matDirection);
    Out.vPosition = float4(In.vPosition, 1.f);
    Out.fIndex = In.fIndex;
    return Out;
}

[maxvertexcount(6)]
void GS_MAIN(line GS_IN In[2], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    matrix matVP;
    float3 vDirection, vUp, vRight;
    
    matVP = mul(g_ViewMatrix, g_ProjMatrix);    

    vDirection = normalize(In[1].vPosition.xyz - In[0].vPosition.xyz);
    vUp = g_vLook * -1.f;
    vRight = normalize(cross(vDirection, vUp)) * g_fWidth * 0.5f;
    
    Out[0].vPosition = float4(In[1].vPosition.xyz - vRight, 1.f);
    Out[1].vPosition = float4(In[1].vPosition.xyz + vRight, 1.f);
    Out[2].vPosition = float4(In[0].vPosition.xyz + vRight, 1.f);
    Out[3].vPosition = float4(In[0].vPosition.xyz - vRight, 1.f);
    
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    
    Out[0].vProjPos = Out[0].vPosition;
    Out[1].vProjPos = Out[1].vPosition;
    Out[2].vProjPos = Out[2].vPosition;
    Out[3].vProjPos = Out[3].vPosition; 
    
    Out[0].vTexcoord = float2(1.f, 1.f);
    Out[1].vTexcoord = float2(0.f, 1.f);
    Out[2].vTexcoord = float2(0.f, 0.f);
    Out[3].vTexcoord = float2(1.f, 0.f);
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
    
}


/* PixelShader */
PS_OUT PS_MAIN_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_vColor;
    
    vColor.rgb = vColor.rgb;
    vColor.a = vColor.a * max(1.f - In.vTexcoord.y, 0.f);

    //float fLuminance = dot(vColor.rgb, g_fBrightness);
    //float fBrightness = max(fLuminance - g_fBloomThreshold, 0.0) / ((length(vColor.rgb) * 0.33f - g_fBloomThreshold));
    if (0.05f > vColor.a)
        discard;

    float fWeight = clamp(10.f / (1e-5 + pow(In.vProjPos.w / 10.f, 3.0f) + pow(In.vProjPos / 200.f, 6.f)), 1e-2, 3e3);
    fWeight = fWeight * max(min(1.0, max(max(vColor.r, vColor.g), vColor.b) * vColor.a), vColor.a);
    
    Out.vAccumulate.rgb = vColor.rgb * vColor.a * fWeight;
    Out.vAccumulate.a = vColor.a * fWeight;
    Out.vRevealage.r = vColor.a /** clamp(log(0.6f + vColor.a), 0.25f, 0.6f)*/;
    
    
    return Out;
}


/* PixelShader */
PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_vColor;
    float4 vMask = g_Texture.Sample(LinearSampler_Clamp, float2(In.vTexcoord.x, In.vTexcoord.y));
    
    vColor.rgb = vColor.rgb;
    vColor.a = vColor.a * max(1.f - In.vTexcoord.y, 0.f) * vMask.r;
    if (0.05f > vColor.a)
        discard;
    
    //float fLuminance = dot(vColor.rgb, g_fBrightness);
    //float fBrightness = max(fLuminance - g_fBloomThreshold, 0.0) / ((length(vColor.rgb) * 0.33f - g_fBloomThreshold));

    float fWeight = clamp(10.f / (1e-5 + pow(In.vProjPos.w / 10.f, 3.0f) + pow(In.vProjPos / 200.f, 6.f)), 1e-2, 3e3);
    fWeight = fWeight * max(min(1.0, max(max(vColor.r, vColor.g), vColor.b) * vColor.a), vColor.a);
    
    Out.vAccumulate.rgb = vColor.rgb * vColor.a * fWeight;
    Out.vAccumulate.a = vColor.a * fWeight;
    Out.vRevealage.r = vColor.a /** clamp(log(0.6f + vColor.a), 0.25f, 0.6f)*/;
    Out.vBloom = Out.vAccumulate;
    
    return Out;
}




technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
    pass Color // 0
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_COLOR();
        ComputeShader = NULL;
    }

    pass DefaultPass // 1
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