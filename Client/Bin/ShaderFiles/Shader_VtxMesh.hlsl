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

cbuffer SingleFresnel : register(b1)
{
    Fresnel OneFresnel;
};

cbuffer ColorBuffer : register(b2)
{
    float4 vDiffuseColor;
    float4 vBloomColor;
    float4 vSubColor;
    float4 vInnerColor;

}

/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_AlbedoTexture, g_NormalTexture, g_ORMHTexture, g_MetallicTexture, g_RoughnessTexture, g_AOTexture, g_SpecularTexture, g_EmissiveTexture; // PBR

Texture2D g_DiffuseTexture, g_NoiseTexture;
float2 g_vDiffuseScaling, g_vNoiseScaling;

float g_fFarZ = 500.f;
float g_fHitRate = 0.f, g_fScalingTime = 0.f;
int g_iFlag = 0;
int g_iRenderFlag = 0;

float4 g_vCamPosition;
float4 g_vDefaultDiffuseColor;
float4 g_vColor, g_vSubColor;
float3 g_vBackGroundColor;
float4 g_vLook;
float g_fBrightness;

float2 g_fStartUV;
float2 g_fEndUV;

/* Trail Data */
float4 g_vTrailColor;
float2 g_vTrailTime;

/* Gray Scale */
int g_isGrayScale = 0;
float g_fGrayScaleColorFactor = 0.12f;
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
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
    Out.vTangent = In.vTangent;
    return Out;
}

VS_OUT VS_TimeScaling(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    matrix matWorldScaling;
    
    float fScaling = 4.f * pow(g_fScalingTime, 2.f);
    
    matWorldScaling[0] = g_WorldMatrix[0] * saturate(fScaling);
    matWorldScaling[1] = g_WorldMatrix[1] * saturate(fScaling);
    matWorldScaling[2] = g_WorldMatrix[2] * saturate(fScaling);
    matWorldScaling[3] = g_WorldMatrix[3];
    

    matWV = mul(matWorldScaling, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), matWorldScaling));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), matWorldScaling);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
    Out.vTangent = In.vTangent;
    
    return Out;    
}


VS_OUT VS_BILLBOARD(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    vector vLookDir = normalize(g_vLook);
    float3 vRightDir = normalize(cross(float3(0.f, 1.0f, 0.f), vLookDir.xyz));
    float3 vUpDir = normalize(cross(vLookDir.xyz, vRightDir));

    float4x4 NewWorld = g_WorldMatrix;
    
    NewWorld[0] = length(g_WorldMatrix[0]) * float4(vRightDir, 0.f);
    NewWorld[1] = length(g_WorldMatrix[1]) * float4(vUpDir, 0.f);
    NewWorld[2] = length(g_WorldMatrix[2]) * float4(vLookDir.xyz, 0.f);
    NewWorld[3] = g_WorldMatrix[3];
    
    matWV = mul(NewWorld, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vPosition = mul(float4(In.vPosition, 1.0), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
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
    
        
    if (g_isGrayScale == 1)
    {
        vAlbedo *= Material.MultipleAlbedo;
        vAlbedo.rgb = dot(vAlbedo.rgb, float3(0.299f, 0.587f, 0.114f)) + (vAlbedo.rgb * g_fGrayScaleColorFactor);
    }
    
   if (vAlbedo.a < 0.01f)
       discard;

    Out.vDiffuse = vAlbedo;
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

PS_OUT PS_BACKGROUND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float4 vAlbedo = g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = In.vNormal.xyz;
    float4 vORMH = float4(0.2f, 0.5f, 0.0f, 0.0f);
    float fSpecular = 0.0f;
    float fEmissive = 0.0f;

    if (vAlbedo.a < 0.1f)
        discard;
    
    Out.vDiffuse = float4((vAlbedo.rgb * g_vBackGroundColor) * g_fBrightness, 1.0f);
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vORMH = vORMH;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, 1.0f);
    float3 vEmissiveColor = float3(0.0f,0.0f,0.0f);
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


struct PS_ACCUM
{
    float4 vAccumulate : SV_TARGET0;
    float vRevealage : SV_TARGET1;
    float4 vBright : SV_TARGET2;
};

float FUNC_WEIGHT(float fDepth, float fAlpha)
{
    return fAlpha * clamp(10.f / (1e-5 + pow(fDepth / 200.f, 4.f)), 1e-2, 3e3);
}

float3 RGBtoHSV(float3 vColor)
{
    float maxC = max(vColor.r, max(vColor.g, vColor.b));
    float minC = min(vColor.r, min(vColor.g, vColor.b));
    float delta = maxC - minC;

    float h = 0;
    if (delta > 0)
    {
        if (maxC == vColor.r)
            h = fmod(((vColor.g - vColor.b) / delta), 6);
        else if (maxC == vColor.g)
            h = ((vColor.b - vColor.r) / delta) + 2;
        else
            h = ((vColor.r - vColor.g) / delta) + 4;
    }
    h = (h < 0 ? h + 6 : h) / 6; // Normalize to [0,1]

    float s = (maxC == 0) ? 0 : delta / maxC;
    float v = maxC;

    return float3(h, s, v);
}

// HSV → RGB 변환
float3 HSVtoRGB(float3 vHsv)
{
    float h = vHsv.x * 6;
    float s = vHsv.y;
    float v = vHsv.z;

    int i = (int) floor(h);
    float f = h - i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    float3 rgb;
    if (i == 0)
        rgb = float3(v, t, p);
    else if (i == 1)
        rgb = float3(q, v, p);
    else if (i == 2)
        rgb = float3(p, v, t);
    else if (i == 3)
        rgb = float3(p, q, v);
    else if (i == 4)
        rgb = float3(t, p, v);
    else
        rgb = float3(v, p, q);

    return rgb;
}

// HSV 보간 함수
float3 HSVLerp(float3 colorA, float3 colorB, float t)
{
    float3 hsvA = RGBtoHSV(colorA);
    float3 hsvB = RGBtoHSV(colorB);

    // Hue는 각도 개념이라 보간 시 회전 방향 고려
    if (abs(hsvA.x - hsvB.x) > 0.5)
    {
        if (hsvA.x > hsvB.x)
            hsvB.x += 1.0;
        else
            hsvA.x += 1.0;
    }

    float3 hsvLerp = lerp(hsvA, hsvB, t);
    hsvLerp.x = fmod(hsvLerp.x, 1.0); // Normalize Hue

    return HSVtoRGB(hsvLerp);
}

PS_ACCUM PS_SINGLEFRESNEL(PS_IN In)
{
    PS_ACCUM Out = (PS_ACCUM) 0;    
    
    float3 vViewDirection = g_vCamPosition.xyz - In.vWorldPos.xyz;
    float fLength = length(vViewDirection);
    vViewDirection = normalize(vViewDirection);
    
    float FresnelValue = Compute_Fresnel(In.vNormal.xyz, vViewDirection, OneFresnel.fBaseReflect, OneFresnel.fExp, OneFresnel.fStrength);
    
    float4 vFresnelColor = OneFresnel.vColor * FresnelValue;

    //Out.vDiffuse.xyz = lerp(vDiffuseColor.xyz, vFresnelColor.xyz, FresnelValue);
    ////Out.vDiffuse = (OneFresnel.vColor * FresnelValue);
    //Out.vDiffuse.w = vDiffuseColor.a;
    //Out.vBrightness = vBloomColor;
    float4 vOutColor;
    vOutColor.a = lerp(vDiffuseColor.a, vFresnelColor.a, FresnelValue);
    vOutColor.rgb = HSVLerp(vDiffuseColor.xyz, vFresnelColor.xyz, FresnelValue);
    
    vOutColor.rgb = HSVLerp(vOutColor.rgb, float3(1.f, 1.f, 1.f), g_fHitRate);
    float3 vOutBloom = HSVLerp(vBloomColor.rgb, float3(1.f, 1.f, 1.f), g_fHitRate);
    
    float fWeight = FUNC_WEIGHT(In.vProjPos.w, vOutColor.a);

    Out.vAccumulate.xyz = vOutColor.rgb * vOutColor.a * fWeight;
    Out.vAccumulate.a = vOutColor.a * fWeight;
    Out.vRevealage.r = vOutColor.a;
    Out.vBright.xyz = vOutBloom * fWeight;
    Out.vBright.a = vBloomColor.a * fWeight;
    //Out.vDiffuse.xyz = lerp(vDiffuseColor.xyz, vFresnelColor.xyz, FresnelValue);
    //Out.vDiffuse = (OneFresnel.vColor * FresnelValue);
    //Out.vDiffuse.w = vDiffuseColor.a;
    //Out.vBrightness = vBloomColor;
    return Out;
}

PS_ACCUM PS_NOISEFRESNEL(PS_IN In)
{
    PS_ACCUM Out = (PS_ACCUM) 0;
    
    float3 vViewDirection = g_vCamPosition.xyz - In.vWorldPos.xyz;
    float fLength = length(vViewDirection);
    vViewDirection = normalize(vViewDirection);
    
    float FresnelValue = Compute_Fresnel(In.vNormal.xyz, vViewDirection, OneFresnel.fBaseReflect, OneFresnel.fExp, OneFresnel.fStrength);
    float4 vFresnelColor = OneFresnel.vColor * FresnelValue;
    
    float vDistortion = g_NoiseTexture.Sample(LinearSampler, float2(In.vTexcoord.x * g_vNoiseScaling.x, In.vTexcoord.y * g_vNoiseScaling.y));
    float vMain = g_DiffuseTexture.Sample(LinearSampler, (In.vTexcoord + float2(vDistortion, vDistortion)) * g_vDiffuseScaling);

    float4 vDefaultColor;
    //vDefaultColor = lerp(vSubColor, vDiffuseColor, vMain);
    vDefaultColor.a = lerp(vSubColor.a, vDiffuseColor.a, vMain);
    vDefaultColor.xyz = HSVLerp(vSubColor.xyz, vDiffuseColor.xyz, vMain);
    
    float fWeight = FUNC_WEIGHT(In.vProjPos.w, vDefaultColor.a);
    
    float4 vOutColor;
    //vOutColor = lerp(vDefaultColor, vFresnelColor, FresnelValue);
    vOutColor.xyz = vDefaultColor.xyz * vDefaultColor.a + vInnerColor.xyz * (1.f - vDefaultColor.a);
    vOutColor.a = vDefaultColor.a;
    //vOutColor = lerp(vOutColor, vFresnelColor, FresnelValue);
    vOutColor.xyz = HSVLerp(vOutColor.xyz, vFresnelColor.xyz, FresnelValue);
    vOutColor.a = lerp(vOutColor.a, vFresnelColor.a, FresnelValue);
    //vOutColor.xyz = vOutColor.xyz * vOutColor.a + vInnerColor.xyz * (1.f - vOutColor.a);
    
    //Out.vDiffuse.xyz = lerp(vMain * vDiffuseColor.xyz, vFresnelColor.xyz, FresnelValue);
    //Out.vDiffuse.xyz = lerp(vDefaultColor.xyz, vFresnelColor.xyz, FresnelValue);
    //Out.vDiffuse.xyz = vDefaultColor.xyz + vFresnelColor.xyz * FresnelValue;
    ////Out.vDiffuse = (OneFresnel.vColor * FresnelValue);
    //Out.vDiffuse.w = vDiffuseColor.a;
    //Out.vBrightness = vBloomColor;
    
    Out.vAccumulate.xyz = vOutColor.rgb * vOutColor.a * fWeight;
    Out.vAccumulate.a = vOutColor.a * fWeight;
    Out.vRevealage.r = vOutColor.a;
    Out.vBright = vBloomColor * fWeight;
    
    return Out;
}

struct PS_TRAIL_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

PS_TRAIL_OUT PS_TRAIL(PS_IN In)
{
    PS_TRAIL_OUT Out = (PS_TRAIL_OUT) 0;
    float fAlpha = 1.0f - g_vTrailTime.y / g_vTrailTime.x;
    float4 FinalColor = g_vTrailColor;
    FinalColor.rgb *= fAlpha;
    FinalColor.a *= fAlpha;

    if (FinalColor.a < 0.01f)
        discard;

    Out.vDiffuse = FinalColor;

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

    pass SingleFresnel // 13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SINGLEFRESNEL();
    }

    pass Background // 14
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BACKGROUND();
    }

    pass TrailPass // 15
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    pass NoiseFresnel // 16
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NOISEFRESNEL();
    }

    pass Fresnel_Scaling // 17
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_TimeScaling();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SINGLEFRESNEL();
    }

    pass NoiseFresnel_Scaling // 18
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_TimeScaling();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_NOISEFRESNEL();
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

