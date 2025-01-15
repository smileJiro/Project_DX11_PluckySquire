#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_DiffuseTexture;

float g_fFarZ = 1000.f;
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

// (장치가 수행)Rendering PipeLine : Projection 변환 (W 나누기 연산 진행) // 
// (장치가 수행)Rendering PipeLine : Viewport 변환 // 
// (장치가 수행)Rendering PipeLine : Rasterization // 

// Rendering PipeLine : PixelShader //
struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL0;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
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

    // 조명에 대한 방향벡터를 뒤집은 후, 노말벡터와의 내적을 통해,
    // shade 값을 구한다. 여기에 Ambient color 역시 더한다. 
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    Out.vDiffuse = vMtrlDiffuse;
    
    // 0.0f ~ 1.0f 사이의 unsigned normal 데이터로 저장한다. 16비트. 추후 해당 범위가 제한적인 상황이 있다면 그때 가서 32비트 float으로 변경한다. 
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.0f);
    
    // vDepth.x = 0 ~ 1 사이의 Depth >> ndc space상의 정규화 된 depth : ndc에서 viewspace로 변환하기 위해 ndc 상의 z를 정의해줘야함. 
    // vDepth.y = 0 ~ 1 사이의 값으로 viewspace 상의 정규화 된 depth : w 나누기했던걸 다시 w 곱해주려고 저장하는 값.
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, 0.0f);
    return Out;
}

// LightDepth 기록용 PixelShader 
struct PS_OUT_LIGHTDEPTH
{
    float vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;
    
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vMtrlDiffuse.a < 0.01f)
        discard;
    
    Out.vLightDepth = In.vProjPos.w / g_fFarZ;
    
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

    pass NoneCullPass
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LightDepth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
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

