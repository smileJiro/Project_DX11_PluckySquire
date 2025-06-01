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


cbuffer SingleFresnel : register(b2)
{
    Fresnel OneFresnel;
};



/* 상수 테이블 */
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
/* Bone Matrix */
float4x4 g_BoneMatrices[256];
Texture2D g_AlbedoTexture, g_NormalTexture, g_ORMHTexture, g_MetallicTexture, g_RoughnessTexture, g_AOTexture, g_SpecularTexture, g_EmissiveTexture; // PBR

float g_fFarZ = 500.f;
int g_iFlag = 0;

float4 g_vCamPosition;
float4x4 g_CamWorld;
float4 g_vOuterColor;
float g_fBaseReflect, g_fExp;

float2 g_fStartUV;
float2 g_fEndUV;

/* Trail Data */
float4 g_vTrailColor;
float2 g_vTrailTime;

/* Texture Blending */
Texture2D g_BlendingTexture;
float g_fBlendingRatio;
/* Gray Scale */
int g_isGrayScale = 0;
float g_fGrayScaleColorFactor = 0.12f;

// hit
float g_fHitRatio = 1.0f;
int g_isHit = 0;


// Vertex Shader //
struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT0;
    uint4 vBlendIndicse : BLENDINDICES;
    float4 vBlendWeights : BLENDWEIGHT;
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
struct VS_SHADOW_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0; // 투영 변환 행렬까지 연산 시킨 포지션 정보를 ps 로 전달한다. >>> w 값을 위해. 
};
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    /* 자신에게 영향을 주는 Bone Matrix를 가중치에 따라 스칼라 곱을 수행한 후, 원소끼리 덧셈연산. */
    /* w 값을 새로 구하는 이유 : 영향받는 Bone 자체가 없는 Fiona의 Sword 같은 메쉬들도 일단 제대로 렌더하기위해. (배치용 Bone) */ 
    float BlendWeightW = 1.0 - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix matBones = mul(g_BoneMatrices[In.vBlendIndicse.x], In.vBlendWeights.x) +
                      mul(g_BoneMatrices[In.vBlendIndicse.y], In.vBlendWeights.y) +
                      mul(g_BoneMatrices[In.vBlendIndicse.z], In.vBlendWeights.z) +
                      mul(g_BoneMatrices[In.vBlendIndicse.w], In.vBlendWeights.w);
    
    vector vPosition = mul(float4(In.vPosition, 1.0), matBones);
    vector vNormal = mul(float4(In.vNormal, 0.0), matBones);
    vector vTangent = mul(float4(In.vTangent, 0.0), matBones);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
    return Out;
}

VS_SHADOW_OUT VS_SHADOWMAP(VS_IN In)
{
    VS_SHADOW_OUT Out = (VS_SHADOW_OUT) 0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    /* 자신에게 영향을 주는 Bone Matrix를 가중치에 따라 스칼라 곱을 수행한 후, 원소끼리 덧셈연산. */
    /* w 값을 새로 구하는 이유 : 영향받는 Bone 자체가 없는 Fiona의 Sword 같은 메쉬들도 일단 제대로 렌더하기위해. (배치용 Bone) */ 
    float BlendWeightW = 1.0 - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);
    
    matrix matBones = mul(g_BoneMatrices[In.vBlendIndicse.x], In.vBlendWeights.x) +
                      mul(g_BoneMatrices[In.vBlendIndicse.y], In.vBlendWeights.y) +
                      mul(g_BoneMatrices[In.vBlendIndicse.z], In.vBlendWeights.z) +
                      mul(g_BoneMatrices[In.vBlendIndicse.w], In.vBlendWeights.w);
    
    vector vPosition = mul(float4(In.vPosition, 1.0), matBones);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition; // w 나누기를 수행하지 않은 0 ~ far 사이의 z 값이 보존되어있는 position
    return Out;
}

VS_OUT VS_MAIN_RENDERTARGET_UV(VS_IN In)
{
    VS_OUT Out = (VS_OUT)0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float BlendWeightW = 1.0 - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix matBones = mul(g_BoneMatrices[In.vBlendIndicse.x], In.vBlendWeights.x) +
        mul(g_BoneMatrices[In.vBlendIndicse.y], In.vBlendWeights.y) +
        mul(g_BoneMatrices[In.vBlendIndicse.z], In.vBlendWeights.z) +
        mul(g_BoneMatrices[In.vBlendIndicse.w], In.vBlendWeights.w);

    vector vPosition = mul(float4(In.vPosition, 1.0), matBones);
    vector vNormal = mul(float4(In.vNormal, 0.0), matBones);
    vector vTangent = mul(float4(In.vTangent, 0.0), matBones);

    float2 vUV = lerp(g_fStartUV, g_fEndUV, In.vTexcoord);

    if (g_iFlag == RT_RENDER_ROTATE)
    {
        float2 vRotate;
        vRotate.x = 1.f - vUV.y;
        vRotate.y = vUV.x;
        vUV = vRotate;
    }

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = vUV;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vTangent, g_WorldMatrix));
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
    VS_WORLDOUT Out = (VS_WORLDOUT) 0;
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    float BlendWeightW = 1.0 - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix matBones = mul(g_BoneMatrices[In.vBlendIndicse.x], In.vBlendWeights.x) +
        mul(g_BoneMatrices[In.vBlendIndicse.y], In.vBlendWeights.y) +
        mul(g_BoneMatrices[In.vBlendIndicse.z], In.vBlendWeights.z) +
        mul(g_BoneMatrices[In.vBlendIndicse.w], In.vBlendWeights.w);
    
    vector vPosition = mul(float4(In.vPosition, 1.0), matBones);
    vector vNormal = mul(float4(In.vNormal, 0.0), matBones);
    vector vTangent = mul(float4(In.vTangent, 0.0), matBones);
    
    // uv를 를 직접 position으로 사용

    float4 vNDCCoord = float4(In.vTexcoord, 0.0f, 1.0f);
    vNDCCoord.x *= 0.5f;
    vNDCCoord.x += g_fStartUV.x;

    if (g_iFlag == RT_RENDER_ROTATE)
    {
        float temp = vNDCCoord.x;
        vNDCCoord.x = 1.f - vNDCCoord.y;
        vNDCCoord.y = temp;
    }

    vNDCCoord = float4(vNDCCoord.xy * 2.0f - 1.0f, 0.0f, 1.0f);
    vNDCCoord.y *= -1.0f;
    
    Out.vPosition = vNDCCoord;
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vWorldNormal = normalize(mul(vNormal, g_WorldMatrix));
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
    float4 vNormal : SV_TARGET1;
    float4 vORMH : SV_TARGET2;
    float4 vDepth : SV_TARGET3;
    float4 vEtc : SV_TARGET4;

};

struct PS_ONLYALBEDO_OUT
{
    float4 vDiffuse : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 조명에 대한 방향벡터를 뒤집은 후, 노말벡터와의 내적을 통해,
    // shade 값을 구한다. 여기에 Ambient color 역시 더한다. 
    float4 vAlbedo = useAlbedoMap ? g_AlbedoTexture.SampleLevel(LinearSampler, In.vTexcoord, 0.0f) : Material.Albedo;
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
    Out.vORMH = vORMH;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, /*렌더플래그자리*/0.0f, 1.0f);
    
    float3 vEmissiveColor = Material.EmissiveColor * fEmissive;
    Out.vEtc = float4(vEmissiveColor, fSpecular /**/);
    
    return Out;
}
PS_OUT PS_MAIN_NONDISCARD(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 조명에 대한 방향벡터를 뒤집은 후, 노말벡터와의 내적을 통해,
    // shade 값을 구한다. 여기에 Ambient color 역시 더한다. 
    float4 vAlbedo = useAlbedoMap ? g_AlbedoTexture.SampleLevel(LinearSampler, In.vTexcoord, 0.0f) : Material.Albedo;
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
    Out.vORMH = vORMH;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, 1.0f);
    
    float3 vEmissiveColor = Material.EmissiveColor * fEmissive;
    Out.vEtc = float4(vEmissiveColor, fSpecular /**/);
    
    return Out;
}


PS_ONLYALBEDO_OUT PS_ONLYALBEDO(PS_IN In) // 포스트 프로세싱 이후에 그리는
{
    PS_ONLYALBEDO_OUT Out = (PS_ONLYALBEDO_OUT) 0;

    float4 vAlbedo = useAlbedoMap ? g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord) : Material.Albedo;
    vAlbedo *= Material.MultipleAlbedo;
    if (0 < g_isHit)
    {
        float fHitRatio = g_fHitRatio;
        fHitRatio = max(pow(fHitRatio, 4.f), 0.2f);

        vAlbedo.rgb = (vAlbedo.rgb * fHitRatio) + (float3(1.0f, 1.0f, 1.0f) * (1.0f - fHitRatio));
        
        //float4 vDir = normalize(mul(float4(0.0f, 0.0f, -1.0f, 0.0f), g_CamWorld));
        //vNormal.xyz = vDir.xyz;
    }
    
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
    PS_WORLDOUT Out = (PS_WORLDOUT) 0;
    
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

    
    Out.vWorldPos = In.vWorldPos;
    Out.vWorldPos.a = fNormalDirectionFlag;
    return Out;
}


struct PS_COLOR
{
    float4 vColor : SV_TARGET0;
};

float Compute_Fresnel(float3 vNormal, float3 vViewDir, float fBaseReflect, float fExponent, float fStrength)
{
    float fNDotV = saturate(dot(vNormal, vViewDir));
    
    float fFresnelFactor = fBaseReflect + (1.f - fBaseReflect) * pow(1 - fNDotV, fExponent);
    
    return saturate(fFresnelFactor * fStrength);
}


PS_COLOR PS_FRESNEL(PS_IN In)
{
    PS_COLOR Out = (PS_COLOR) 0;
    
    float3 vViewDirection = g_vCamPosition.xyz - In.vWorldPos.xyz;
    float fLength = length(vViewDirection);
    vViewDirection = normalize(vViewDirection);
    
    float InnerValue = Compute_Fresnel(-In.vNormal.xyz, vViewDirection, InnerFresnel.fBaseReflect, InnerFresnel.fExp, InnerFresnel.fStrength);
    float OuterValue = Compute_Fresnel(In.vNormal.xyz, vViewDirection, OuterFresnel.fBaseReflect, OuterFresnel.fExp, OuterFresnel.fStrength);
    
    Out.vColor = (InnerFresnel.vColor * InnerValue + OuterValue * OuterFresnel.vColor)
    * fLength;
    Out.vColor.a = 1.f;

 
    //Out.vBloom = float4(Out.vColor.rgb * fBrightness, Out.vColor.a * fBrightness);
    
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


struct PS_TRAIL_OUT
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



PS_TRAIL_OUT PS_TRAIL(PS_IN In)
{
    PS_TRAIL_OUT Out = (PS_TRAIL_OUT) 0;     
    
    float3 vViewDirection = g_vCamPosition.xyz - In.vWorldPos.xyz;
    float fLength = length(vViewDirection);
    vViewDirection = normalize(vViewDirection);
    
    float FresnelValue = Compute_Fresnel(In.vNormal.xyz, vViewDirection, g_fBaseReflect, g_fExp, 1.f);    
    
    float3 vFresnelColor = HSVLerp(g_vTrailColor.xyz, g_vOuterColor.xyz, FresnelValue);
    
    float fAlpha = 1.0f - g_vTrailTime.y / g_vTrailTime.x;
    float4 FinalColor/* = g_vTrailColor*/;
    FinalColor.rgb = vFresnelColor;
    FinalColor.a = g_vTrailColor.a;
    
    FinalColor.rgb *= fAlpha;
    FinalColor.a *= fAlpha;

    if (FinalColor.a < 0.01f)
        discard;
    
    float fWeight = FUNC_WEIGHT(In.vProjPos.w, FinalColor.a);
    //float fWeight = 300.f;
    Out.vAccumulate.xyz = FinalColor.xyz * FinalColor.a * fWeight;
    Out.vAccumulate.a = FinalColor.a * fWeight;
    Out.vRevealage.r = FinalColor.a;
    Out.vBright = Out.vAccumulate;
    
    //Out.vBloom = FinalColor;
    return Out;
}

PS_OUT PS_MAIN_TEXTUREBLENDING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // 조명에 대한 방향벡터를 뒤집은 후, 노말벡터와의 내적을 통해,
    // shade 값을 구한다. 여기에 Ambient color 역시 더한다. 
    float4 vAlbedo = useAlbedoMap ? g_AlbedoTexture.SampleLevel(LinearSampler, In.vTexcoord, 0.0f) : Material.Albedo;
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
    
    /* Blending할 텍스처 샘플링 */
    float4 vBlendingAlbedo = g_BlendingTexture.SampleLevel(LinearSampler, In.vTexcoord, 0.0f);
    vAlbedo = lerp(vAlbedo, vBlendingAlbedo, g_fBlendingRatio); // Ratio 0 -> 1
    
    Out.vDiffuse = vAlbedo * Material.MultipleAlbedo;
    // 1,0,0 
    // 1, 0.5, 0.5 (양의 x 축)
    // 0, 0.5, 0.5 (음의 x 축)
    Out.vNormal = float4(vNormal.xyz * 0.5f + 0.5f, 1.f);
    Out.vORMH = vORMH;
    Out.vDepth = float4(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_fFarZ, 0.0f, 1.0f);
    
    float3 vEmissiveColor = Material.EmissiveColor * fEmissive;
    Out.vEtc = float4(vEmissiveColor, fSpecular /**/);
    
    return Out;
}

// technique : 셰이더의 기능을 구분하고 분리하기 위한 기능. 한개 이상의 pass를 포함한다.
// pass : technique에 포함된 하위 개념으로 개별 렌더링 작업에 대한 구체적인 설정을 정의한다.
// https://www.notion.so/15-Shader-Keyword-technique11-pass-10eb1e26c8a8807aad86fb2de6738a1a // 컨트롤 클릭
technique11 DefaultTechnique
{
	/* 우리가 수행해야할 정점, 픽셀 셰이더의 진입점 함수를 지정한다. */
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

    pass RenderTargetMappingPass // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_RENDERTARGET_UV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONDISCARD();
    }

    pass RenderTargetMappingPass_AfterPostProcessing // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN_RENDERTARGET_UV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_ONLYALBEDO();
    }

    pass BookWorldPosMap // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_BOOKWORLDPOSMAP();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WORLDPOSMAP();
    }
    
    pass Fresnel // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FRESNEL();
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
    pass ShadowMap // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_SHADOWMAP();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SHADOWMAP();
    }
    pass TrailPass // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_WriteNone, 0);
        SetBlendState(BS_WeightAccumulate, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    pass TextureBlendingPass // 11 -> Book에서 사용
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_RENDERTARGET_UV();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_TEXTUREBLENDING();
    }

}