#include "../../../EngineSDK/hlsl/Engine_Shader_Define.hlsli"
#include "../../../EngineSDK/hlsl/Engine_Shader_Function.hlsli"

/* PS ConstBuffer */ 
cbuffer BasicPixelConstData : register(b0)
{
    Material_PS Material; // 32
    
    int useAlbedoMap;
    int useNormalMap;
    int useAOMap;
    int useMetallicMap; // 16
    
    int useRoughnessMap;
    int useEmissiveMap;
    int useORMHMap;
    int useSpecularMap; // 16
    
    int invertNormalMapY; 
    float3 dummy;
}

struct Fresnel
{
    float4 vColor;
    
    float fExp;
    float fBaseReflect;
    float fStrength;
    float fDummy;
};

cbuffer MultiFresnels : register(b1)
{
    Fresnel InnerFresnel;
    Fresnel OuterFresnel;
}



/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_AlbedoTexture, g_NormalTexture, g_ORMHTexture, g_MetallicTexture, g_RoughnessTexture, g_AOTexture, g_SpecularTexture, g_EmissiveTexture; // PBR

float g_fFarZ = 500.f;
int g_iFlag = 0;
int g_iRenderFlag = 0;

float4 g_vCamPosition;
float4 g_vDefaultDiffuseColor;
float4 g_vColor, g_vSubColor;

float2 g_fStartUV;
float2 g_fEndUV;

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
    float3 vTangent : TEXCOORD3;
};
struct VS_SHADOW_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0; // 투영 변환 행렬까지 연산 시킨 포지션 정보를 ps 로 전달한다. >>> w 값을 위해. 
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
    Out.vTangent = In.vTangent;
    return Out;
}

VS_SHADOW_OUT VS_SHADOWMAP(VS_IN In)
{
    VS_SHADOW_OUT Out = (VS_SHADOW_OUT) 0;
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
    return Out;
}

struct VS_WORLDOUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vWorldNormal : TEXCOORD2;
};

VS_WORLDOUT VS_BOOKWORLDPOSMAP(VS_IN In)
{
    VS_WORLDOUT Out = (VS_WORLDOUT)0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    
    float2 vTexCoord = saturate(In.vTexcoord);
    
    
    //0~1 사이로 전환.
    if (g_iFlag == RT_RENDER_FRAC || g_iFlag == RT_RENDER_UVRENDER_FRAC)
    {
        vTexCoord = frac(In.vTexcoord);
    }
    
    float4 vNDCCoord = float4(vTexCoord.x, vTexCoord.y, 0.0f, 1.0f);

    //Start와 End를 사용함.
    if (g_iFlag == RT_RENDER_UVRENDER || g_iFlag == RT_RENDER_UVRENDER_FRAC)
    {
        float2 vUV = lerp(g_fStartUV, g_fEndUV, vTexCoord);
        vNDCCoord.xy = vUV.xy;
    }
    
    if (g_iFlag == RT_RENDER_ROTATE)
    {
        float temp = vNDCCoord.x;
        vNDCCoord.x = 1.f - vNDCCoord.y;
        vNDCCoord.y = temp;
    }

    
    
    vNDCCoord = float4(vNDCCoord.xy * 2.0f - 1.0f, 0.0f, 1.0f);
    vNDCCoord.y *= -1.0f;
    Out.vPosition = vNDCCoord;
    Out.vTexcoord = vTexCoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    Out.vWorldNormal = mul(float4(In.vNormal, 0.0f), g_WorldMatrix);
    return Out;
}

VS_OUT VS_MAIN_RENDERTARGET_UV(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    //float2 vUV = g_fStartUV + In.vTexcoord * (g_fEndUV - g_fStartUV);
    float2 vUV = lerp(g_fStartUV, g_fEndUV, In.vTexcoord);

    if (g_iFlag == RT_RENDER_ROTATE)
    {
        float2 vRotate;
        vRotate.x = 1.f - vUV.y;
        vRotate.y = vUV.x;
        vUV = vRotate;
    }
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), g_WorldMatrix));
    Out.vTexcoord = vUV;
    Out.vWorldPos = mul(Out.vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
    Out.vTangent = In.vTangent;

    return Out;
}


// Rendering PipeLine : PixelShader //
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
    float4 vNormal : SV_TARGET1;
    float4 vORMH : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vEtc : SV_TARGET4;
};

struct PS_ONLYALBEDO_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

/* PixelShader */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vAlbedo = useAlbedoMap ? g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord) : Material.Albedo;
    float3 vNormal = useNormalMap ? Get_WorldNormal(g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz, In.vNormal.xyz, In.vTangent.xyz, 0) : In.vNormal.xyz;
    float4 vORMH = useORMHMap ? g_ORMHTexture.Sample(LinearSampler, In.vTexcoord) : float4(Material.AO, Material.Roughness, Material.Metallic, 1.0f);
    float fSpecular = useSpecularMap ? g_SpecularTexture.Sample(LinearSampler, In.vTexcoord).r : 0.0f;
    float fEmissive = useEmissiveMap ? g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord).r : Material.Emissive;
    if (false == useORMHMap)
    {
        vORMH.r = useAOMap ? g_AOTexture.Sample(LinearSampler, In.vTexcoord).r : Material.AO;
        vORMH.g = useRoughnessMap ? g_RoughnessTexture.Sample(LinearSampler, In.vTexcoord).r : Material.Roughness;
        vORMH.b = useMetallicMap ? g_MetallicTexture.Sample(LinearSampler, In.vTexcoord).r : Material.Metallic;
    }
    
   if (vAlbedo.a < 0.1f)
       discard;
    
    Out.vDiffuse = vAlbedo * Material.MultipleAlbedo;
    // 1,0,0
    // 1, 0.5, 0.5 (양의 x 축)
    // 0, 0.5, 0.5 (음의 x 축)
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);

    //Out.vDiffuse = 1.0f; // Test Code5
    //vORMH.g = 0.02f;// TestCode
    //vORMH.b = 1.00f;// TestCode
    Out.vORMH = vORMH;
    float fRenderFlag = (float)g_iRenderFlag;
    if (g_iRenderFlag & SAVEBRDF != 0)
    {

        fSpecular = 1.0f;
    }
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, fRenderFlag, 1.0f);
    float3 vEmissiveColor = Material.EmissiveColor * fEmissive;
    Out.vEtc = float4(vEmissiveColor, fSpecular);
    
    return Out;
}

PS_ONLYALBEDO_OUT PS_ONLYALBEDO(PS_IN In) // 포스트 프로세싱 이후에 그리는
{
    PS_ONLYALBEDO_OUT Out = (PS_ONLYALBEDO_OUT) 0;

    float4 vAlbedo = useAlbedoMap ? g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord) : Material.Albedo;
    
    if (vAlbedo.a < 0.01f)
        discard;

    Out.vDiffuse = vAlbedo;
    
    return Out;
}

struct PS_SHADOW_IN
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};


// LightDepth 기록용 PixelShader 
struct PS_SHADOWMAP_OUT
{
    float vLightDepth : SV_TARGET0;
};

PS_SHADOWMAP_OUT PS_SHADOWMAP(PS_SHADOW_IN In)
{
    PS_SHADOWMAP_OUT Out = (PS_SHADOWMAP_OUT) 0;

    
    Out.vLightDepth = In.vProjPos.w;
    
    return Out;
}

PS_OUT PS_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    //Out.vDiffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
    Out.vDiffuse = g_vDefaultDiffuseColor;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    float fFlag = g_iFlag;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, fFlag);
    
    return Out;
}


PS_OUT PS_MIX_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord);
    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    //Out.vDiffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
    Out.vDiffuse = saturate(vMtrlDiffuse * g_vDefaultDiffuseColor);
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    float fFlag = g_iFlag;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, fFlag);
    
    return Out;
}

PS_OUT PS_TEST_PROJECTILE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vMtrlDiffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    Out.vDiffuse = float4(0.0f, 1.0f, 0.0f, 1.0f);
    //Out.vDiffuse = g_vDefaultDiffuseColor;
    Out.vNormal = float4(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    float fFlag = g_iFlag;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, fFlag);
    
    return Out;
}

struct PS_PLAYERDEPTHOUT
{
    float fPlayerDepth : SV_TARGET0;
};

PS_PLAYERDEPTHOUT PS_PLAYERDEPTH(PS_IN In)
{
    PS_PLAYERDEPTHOUT Out = (PS_PLAYERDEPTHOUT) 0;

    Out.fPlayerDepth = In.vProjPos.w / g_fFarZ;
    return Out;
}

struct PS_WORLDIN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float3 vWorldNormal : TEXCOORD2;
};
struct PS_WORLDOUT
{
    float4 vWorldPos : SV_TARGET0;
};

PS_WORLDOUT PS_WORLDPOSMAP(PS_WORLDIN In)
{
    PS_WORLDOUT Out = (PS_WORLDOUT)0;
    
    float fNormalDirectionFlag = NONEWRITE_NORMAL;
    // +X, -X
    if (pow(In.vWorldNormal.x, 2) > pow(In.vWorldNormal.y, 2) && pow(In.vWorldNormal.x, 2) > pow(In.vWorldNormal.z, 2))
    {
        fNormalDirectionFlag = (In.vWorldNormal.x) > 0 ? POSITIVE_X : NEGATIVE_X;
    }
    // +Y, -Y
    else if (pow(In.vWorldNormal.y, 2) > pow(In.vWorldNormal.x, 2) && pow(In.vWorldNormal.y, 2) > pow(In.vWorldNormal.z, 2))
    {
        fNormalDirectionFlag = (In.vWorldNormal.y) > 0 ? POSITIVE_Y : NEGATIVE_Y;
    }
    // +Z, -Z
    else if (pow(In.vWorldNormal.z, 2) > pow(In.vWorldNormal.x, 2) && pow(In.vWorldNormal.z, 2) > pow(In.vWorldNormal.y, 2))
    { 
        fNormalDirectionFlag = (In.vWorldNormal.z) > 0 ? POSITIVE_Z : NEGATIVE_Z;
    }

    Out.vWorldPos.rgb = In.vWorldPos.rgb;
    Out.vWorldPos.a = fNormalDirectionFlag;

    return Out;
}

float Compute_Fresnel(float3 vNormal, float3 vViewDir, float fBaseReflect, float fExponent, float fStrength)
{
    float fNDotV = saturate(dot(vNormal, vViewDir));
    
    float fFresnelFactor = fBaseReflect + (1.f - fBaseReflect) * pow(1 - fNDotV, fExponent);
    
    return saturate(fFresnelFactor * fStrength);
}

struct PS_DIFFUSE
{
    float4 vColor : SV_TARGET0;
};

PS_DIFFUSE PS_MAIN2(PS_IN In)
{
    PS_DIFFUSE Out = (PS_DIFFUSE) 0;

    float4 vAlbedo = g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
   
    Out.vColor = vAlbedo;
    //Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    //Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, 0.f);

    return Out;
}


struct PS_BLOOM
{
    float4 vDiffuse : SV_TARGET0;
    float4 vBrightness : SV_TARGET1;
};

PS_BLOOM PS_EMISSIVE(PS_IN In)
{
    PS_BLOOM Out = (PS_BLOOM) 0;
        
    float4 vColor = g_vColor;
    
    Out.vDiffuse = vColor;
    Out.vBrightness = g_vSubColor;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass DefaultPass // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass AfterPostProcessingPass // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ONLYALBEDO();
    }

    pass AlphaPass // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ONLYALBEDO();
    }

    pass NoneCullPass // 3
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Color // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

    pass MixColor // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MIX_COLOR();
    }

    pass TestProjectile //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TEST_PROJECTILE();
    }

    pass BookWorldPosMap // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BOOKWORLDPOSMAP();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WORLDPOSMAP();
    }

    pass PlayerDepth // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PLAYERDEPTH();
    }

    pass RenderTargetMappingPass // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_RENDERTARGET_UV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass RenderTargetMappingPass_AfterPostProcessing // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_RENDERTARGET_UV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ONLYALBEDO();
    }

    pass ShadowMap // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SHADOWMAP();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHADOWMAP();
    }

    pass Emissive // 12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_EMISSIVE();
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

