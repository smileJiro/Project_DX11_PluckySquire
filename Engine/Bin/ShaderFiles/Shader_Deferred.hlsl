#include "Engine_Shader_Define.hlsli"
#include "Engine_Shader_Function.hlsli"

/* Global IBL ConstBuffer */ 
struct GlobalIBLData
{
    float fStrengthIBL;             // IBL의 전체적인 영향 빈도를 결정.(간접광 자체의 강도라고 보면 될 듯함.)
    int iSpecularBaseMipLevel;      // Specular MipLevel의 시작레벨 >>> Default 2
    float fRoughnessToMipFactor;    // Roughness가 Specular에 얼마나 영향을 주는가.
    float fHDRMaxLuminance;         // HDRI Target의 최대 밝기(휘도)
    
    int iToneMappingFlag;           // 어떤 톤매핑을 사용할 것인가.
    float fExposure;                // 노출시간
    float fGamma;                   // 감마
    float dummy1;                
};

/* DirectLight ConstBuffer */ 
struct DirectLightData
{
    float3 vRadiance; // 빛의 세기와 컬러
    float fFallOutStart; // 감쇠 시작 거리 (16byte)

    float3 vDirection; // 빛의 방향
    float fFallOutEnd; // 감쇠 끝 거리 (조명의 Range 역할) (16byte)

    float3 vPosition; // 빛의 위치
    float fSpotPower; // 스포트라이트의 집중도 (16byte)

    // 후보정 값 (분위기 연출용)
    float4 vDiffuse; // 난반사광 (16byte)
    float4 vAmbient; // 주변광 (16byte)
    float4 vSpecular; // 정반사광 (16byte)
    
    int isShadow;
    float fLightRadius;
    float fShadowFactor;
    float dummy1;
    
    matrix LightViewProjMatrix;
};

/* DofData */ 
struct DofData
{
    float fSensorHeight;
    float fAperture; // 조리개 크기
    float fFocusDistance; // 초점 평면거리
    float fFocalLength; // Fovy와 FocusDistance를 기반으로 구해지는 값.
    
    float fDofBrightness;
    float fBaseBlurPower; // 기본 전체 블러값
    float fFadeRatio; // FadeIn, FadeOut Ratio
    float dummy2;
    
    float3 vBlurColor;
    float dummy3;
};


cbuffer GlobalIBLConstData : register(b0)
{
    GlobalIBLData c_GlobalIBLVariable;
};
cbuffer BasicDirectLightConstData : register(b1)
{
    DirectLightData c_DirectLight;
}
cbuffer DofConstData : register(b2)
{
    DofData c_DofVariable;
}

float2 g_RandomTexcoords[64];
/* Basic */
// Object Matrix Data 
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_CamViewMatrix, g_CamProjMatrix;
float4x4 g_ViewMatrixInv, g_ProjMatrixInv;
float4x4 g_LightViewMatrix, g_LightProjMatrix;
// Debug
Texture2D g_Texture;
// Geometry RTV
Texture2D g_AlbedoTexture, g_NormalTexture, g_ORMHTexture, g_DepthTexture;
// PlayerDepth
Texture2D g_PlayerDepthTexture;
// Env Map
TextureCube g_IBLSpecularTexture, g_IBLIrradianceTexture; // Irradiance = diffuse
Texture2D g_BRDFTexture;
// 직접광원 결과물
Texture2D g_DirectLightsTexture;
// Lighting 결과물 
Texture2D g_LightingTexture;
// Blur 대상 Texture2D
Texture2D g_SrcBlurTargetTexture;
Texture2D g_PostProcessingTexture;
// etc
Texture2D g_ShadowMapTexture, g_FinalTexture;
// Default Mtrl
vector g_vMtrlAmbient = 1.0f, g_vMtrlSpecular = 1.0f; // 특별한 경우가 아니라면 재질의 Ambient와 Specular는 특정 값으로 고정 후, 조명의 값으로 변화를 준다. 
float4 g_vCamWorld;
float g_fFarZ = 1000.f;

float3 g_vHideColor;

/* Effect */
// Weighted Blended
Texture2D g_AccumulateTexture, g_RevealageTexture;

// Effect Texture
Texture2D g_EffectColorTexture, g_BloomTexture, g_BloomTexture2, g_DistortionTexture;

static const float3 Fdielectric = 0.04f;
//Texture2D g_EffectTexture, g_Effect_BrightnessTexture, g_Effect_Blur_XTeuxture, g_Effect_Blur_YTeuxture, g_Effect_DistortionTeuxture;

// Blur 
float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

float NdfGGX(float _NdotH, float _fRoughness) // 거칠기에 따라 표면의 미세입자들이 얼마나 우리의 눈으로 향하는가 (노말분포함수 : 확통 정규분포랑 좀 다름)
{
    float fAlpha = _fRoughness * _fRoughness;
    float fAlphaSquare = fAlpha * fAlpha;
    float fDenom = (_NdotH * _NdotH) * (fAlphaSquare - 1.0f) + 1.0f;
    
    return fAlphaSquare / (PI * fDenom * fDenom);
}

// Single term for separable Schlick-GGX below.
float SchlickG1(float _NdotV, float _k)
{
    return _NdotV / (_NdotV * (1.0f - _k) + _k);
}

// Schlick-GGX 근사를 사용한 Smith 방법 기반의 기하 감쇠 함수
float SchlickGGX(float _NdotI, float _NdotO, float _fRoughness)
{
    float r = _fRoughness + 1.0;
    float k = (r * r) / 8.0;
    return SchlickG1(_NdotI, k) * SchlickG1(_NdotO, k);
}

float3 SchlickFresnel(float3 _F0, float _NdotH) // 물체의 반사계수 F를 구하는 함수.
{
    // 바라보는 방향과 물체의 노말이 평행할수록, >>> 반사계수가 낮고, 원래 물체의 고유색상을 띈다.
    // 노말이 수직일 수록 >>> 물체 고유색을 잃고 반사계수가 커지는 것.
    return _F0 + (1.0f - _F0) * pow(2.0f, (-5.55473f * _NdotH - 6.98316f) * _NdotH); // 성능상 유리한.
    //return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); // 원형
}

float3 DiffuseIBL(float3 _vAlbedo, float3 _vNormalWorld, float3 _vPixelToEye, float _fMetallic)
{
    // 핵심 : 물체의 반사계수를 프레넬 공식으로 구하고, (1- 반사계수) 만큼의 에너지를 확산반사로 사용하는 것임 그래서 kd를 구하고 kd * 알베도 * 디퓨즈맵 = 최종 diffuse color 
    // Func Shlick(F0, l, n); l : 
    float3 F0 = lerp(Fdielectric, _vAlbedo, _fMetallic); // fMetallic 값을 가지고 보간한다. FDielectric은 0.04f;
    float F = SchlickFresnel(F0, dot(_vNormalWorld, _vPixelToEye));
    float3 kd = lerp(1.0 - F, 0.0, _fMetallic); // 물체의 확산반사계수 : 메탈릭 수치에 따라 보정되는 값임 >>> F로 반사되는 에너지를 제외한 에너지를 확산반사의 양으로 사용함.
    float3 vIrradiance = g_IBLIrradianceTexture.Sample(LinearSampler, _vNormalWorld).rgb; // Diffuse는 물체의 노말방향에서 샘플링.
    
    return kd * _vAlbedo * vIrradiance;
}
float3 SpecularIBL(float3 _vAlbedo, float3 _vNormalWorld, float3 _vPixelToEye, float _fMetallic, float _fRoughness)
{
    // 1.0f - Roughness >>> IBL Baker의 텍스쳐가 반전되어있어서 적용.
    float2 vSpecularBRDF = g_BRDFTexture.Sample(LinearSampler_Clamp, float2(dot(_vNormalWorld, _vPixelToEye), 1.0f - _fRoughness)).rg; // BRDF 함수를 통한 물체의 반사빈도를 계산. 거칠기가 높을수록 적은 값.
    float3 vSpecularIrradiance = g_IBLSpecularTexture.SampleLevel(LinearSampler, reflect(-_vPixelToEye, _vNormalWorld), c_GlobalIBLVariable.iSpecularBaseMipLevel + _fRoughness * c_GlobalIBLVariable.fRoughnessToMipFactor /*스페큘러 반사에대한*/).rgb;
    
    float3 F0 = lerp(Fdielectric, _vAlbedo, _fMetallic);
    
    return (F0 * vSpecularBRDF.x + vSpecularBRDF.y) * vSpecularIrradiance;
}

float3 AmbientLightingByIBL(float3 _vAlbedo, float3 _vNormalWorld, float3 _vPixelToEye, float _fAO,
                            float _fMetallic, float _fRoughness)
{
    float3 vDiffuseIBL = DiffuseIBL(_vAlbedo, _vNormalWorld, _vPixelToEye, _fMetallic);
    float3 vSpecularIBL = SpecularIBL(_vAlbedo, _vNormalWorld, _vPixelToEye, _fMetallic, _fRoughness);
     
    return (vDiffuseIBL + vSpecularIBL) * _fAO;
}

float3 LinearToneMapping(float3 _vColor) // 기본적인 선형 톤매핑
{
    float3 vInvGamma = float3(1, 1, 1) / c_GlobalIBLVariable.fGamma;

    _vColor = clamp(c_GlobalIBLVariable.fExposure * _vColor, 0., 1.);
    _vColor = pow(_vColor, vInvGamma);
    
    return _vColor;
}
float3 FilmicToneMapping(float3 _vColor) // 영화같은 분위기를 연출 부드러운 톤매핑
{
    _vColor = max(float3(0.0f, 0.0f, 0.0f), _vColor);
    _vColor = (_vColor * (6.2f * _vColor + 0.5f)) / (_vColor * (6.2f * _vColor + 1.7f) + 0.06f);
    return _vColor;
}
float3 Uncharted2ToneMapping(float3 _vColor) // 언챠티드 시리즈에서 개발한 톤매핑
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    
    _vColor *= c_GlobalIBLVariable.fExposure;
    _vColor = ((_vColor * (A * _vColor + C * B) + D * E) / (_vColor * (A * _vColor + B) + D * F)) - E / F;
    float fWhite = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    _vColor /= fWhite;
    _vColor = pow(_vColor, float3(1.0, 1.0, 1.0) / c_GlobalIBLVariable.fGamma);
    return _vColor;
}

float3 lumaBasedReinhardToneMapping(float3 _vColor) // 색상의 변화가 적은 부드러운 톤매핑
{
    float3 vInvGamma = float3(1, 1, 1) / c_GlobalIBLVariable.fGamma;
    float fLuma = dot(_vColor, float3(0.2126, 0.7152, 0.0722));
    float fToneMappedLuma = fLuma / (1. + fLuma);
    _vColor *= fToneMappedLuma / fLuma;
    _vColor = pow(_vColor, vInvGamma);
    return _vColor;
}

//float3 LightRadiance(DirectLightData _tLight, float3 vPixelWorldPos, float3 _vWorldNormal, Texture2D shadowMap)
//{
//    float3 vLight
//}

float4 GetWorldPositionFromDepth(float2 _vTexcoord, float _fNDCDepth, float _fViewSpaceZ) // 뎁스값을 기반으로 픽셀의 월드포지션을 연산.
{
    float4 vPixelWorld = 0.0f;
    /* 1. 픽셀의 위치(Texcoord)를 NDC 공간의 좌표로 변환  */
    vPixelWorld.x = _vTexcoord.x * 2.f - 1.f;
    vPixelWorld.y = _vTexcoord.y * -2.f + 1.f;
    vPixelWorld.z = _fNDCDepth;
    vPixelWorld.w = 1.f;
    /* 2. W 나누기 된 값이니 다시 FarZ를 곱하여준다. */
    vPixelWorld *= _fViewSpaceZ;
    /* 3. 투영행렬을 곱하여 뷰스페이스 상의 좌표로 이동. */
    vPixelWorld = mul(vPixelWorld, g_ProjMatrixInv);
    /* 4. 뷰스페이스행렬을 곱하여 월드스페이스 상의 좌표로 이동. (끝)*/
    vPixelWorld = mul(vPixelWorld, g_ViewMatrixInv);
    
    return vPixelWorld;
}

float PCF_Filter(float2 _vUV, float _fZReceiverNDC, float _fFilterRadiusUV, Texture2D _ShadowMapTexture)
{
    // RTV 기반 쉐도우맵이라 직접 PCF를 구현하여야 한다.
    int iKernelSize = 8;
    float fSumShadowFactor = 0.0f;

    // 커널 중심에서 주변 텍셀 샘플링
    int iLoopNum = (iKernelSize / 2);
    for (int x = -iLoopNum; x <= iLoopNum; ++x) // '<='로 수정
    {
        for (int y = -iLoopNum; y <= iLoopNum; ++y) // '<='로 수정
        {
            // 주변 텍셀 좌표 계산
            int DiskY = y + iLoopNum;
            int DiskX = x + iLoopNum;

            // Poisson Disk 인덱스 순환 접근
            int poissonIndex = (DiskY * iKernelSize + DiskX) % 64;
            float2 vOffset = diskSamples64[poissonIndex] * _fFilterRadiusUV;

            // 쉐도우맵에서 깊이 샘플링
            float fSampledDepth = _ShadowMapTexture.Sample(LinearSampler_Clamp, _vUV + vOffset);

            // 깊이 비교 (좌표계 확인 필요)
            if (fSampledDepth <= _fZReceiverNDC)
            {
                fSumShadowFactor += c_DirectLight.fShadowFactor;
            }
        }
    }

    // 평균화하여 부드러운 그림자 생성
    return (fSumShadowFactor) / (iKernelSize * iKernelSize); // 동적 커널 크기 지원
}

float3 RandomWorldHemiSphereNormal(float3 _vWorldNormal, float2 _vTexCoord)
{
// UV 좌표 기반 난수 생성
    float NoiseX = frac(sin(dot(_vTexCoord, float2(15.8989f, 76.132f))) * 46446.23745f) * 2.0f - 1.0f;
    float NoiseY = frac(sin(dot(_vTexCoord, float2(11.9899f, 62.223f))) * 34748.34744f) * 2.0f - 1.0f;
    float NoiseZ = frac(sin(dot(_vTexCoord, float2(13.3289f, 63.122f))) * 59998.47362f) * 2.0f - 1.0f;

// 로컬 좌표계에서 랜덤 벡터 생성
    float3 randomVec = normalize(float3(NoiseX, NoiseY, abs(NoiseZ))); // Z축을 양수로 제한

// TBN 행렬로 법선 기준 정렬
    float3 upVector = abs(_vWorldNormal.y) < 0.999 ? float3(0.f, 1.f, 0.f) : cross(float3(0, 1, 0), _vWorldNormal);
    float3 tangent = normalize(cross(_vWorldNormal, upVector)); // Tangent 계산
    float3 bitangent = cross(_vWorldNormal, tangent); // Bitangent 계산
    float3x3 TBN = float3x3(tangent, bitangent, _vWorldNormal); // TBN 행렬 생성

// 로컬 랜덤 벡터를 법선 기준으로 변환
    return mul(TBN, randomVec);
}
float3 SampleHemisphere(float3 randomNormal, float3 worldNormal)
{

    // 로컬 좌표계에서의 샘플 방향
    float3 localDir = randomNormal;

    // 2. 월드 좌표계를 기준으로 변환
    // worldNormal을 기준으로 직교 좌표계 생성
    float3 tangent = normalize(abs(worldNormal.z) > 0.999 ? float3(1, 0, 0) : cross(float3(0, 1, 0), worldNormal));
    float3 bitangent = cross(worldNormal, tangent);

    // 로컬 방향을 월드 공간으로 변환
    return localDir.x * tangent + localDir.y * bitangent + localDir.z * worldNormal;
}
float Compute_SSAO(float3 _vPixelWorldPos, float3 _vPixelWorldNormal, float2 _vRandomTexcoord, float _fRadius, Texture2D _DepthTexture, float3 _vRandomNormal)
{
    float3 vViewPosition = mul(float4(_vPixelWorldPos, 1.0f), g_CamViewMatrix).xyz;
    float3 vViewNormal = mul(float4(_vPixelWorldNormal, 0.0f), g_CamViewMatrix).xyz;
    
    float3 vRandomWorldHemiNormal = SampleHemisphere(_vRandomNormal, _vPixelWorldNormal);


    float3 vRandomViewHemiNormal = mul(float4(vRandomWorldHemiNormal, 0.0f), g_CamViewMatrix).xyz;
    
    float flip = sign(dot(vRandomViewHemiNormal, vViewNormal)); // 부호 결정
    
    float3 vRandomViewPos = vViewPosition + (normalize(vRandomViewHemiNormal) /** flip */* _fRadius);
    float4 vNDCPos = mul(float4(vRandomViewPos, 1.0f), g_CamProjMatrix);  
    vNDCPos.xyz /= vNDCPos.w;
    float2 vRandomPosTexcoord = vNDCPos.xy;
    vRandomPosTexcoord = float2(vRandomPosTexcoord.x, vRandomPosTexcoord.y * -1.0f);
    vRandomPosTexcoord += 1.0f;
    vRandomPosTexcoord *= 0.5f;
    
    float fSampleViewZ = g_DepthTexture.Sample(LinearSampler_Clamp, vRandomPosTexcoord).y * g_fFarZ;
    float fBiaseFactor = 0.0001f;
    float bias = fBiaseFactor * vViewPosition.z / g_fFarZ;
    float dp = max(dot(vViewNormal, normalize(vRandomViewPos - (vViewPosition))), 0.0f); // 새로 샘플한 지점의 뷰 공간 포지션과 현재 나의 노말을 내적하여 방향성을 비교
    float fDistZ = saturate(((vViewPosition.z - bias - fSampleViewZ) / _fRadius)); // 뷰스페이스의 Z 값을 정규화 

    float zFalloff = 2.0f;
    float zFactor = saturate(1.0f - (abs((vViewPosition.z - bias - fSampleViewZ)) / zFalloff)) * 1.2f;

    float fSSAO = dp * fDistZ * zFactor;
    return fSSAO;
}

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION; // POSITION 정보는 추후 W 나누기를 장치가 내부적으로 수행하기때문에, W값을 보존하여야한다. 1.0f로 리턴.
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPositon : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_PBR_LIGHT_POINT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    /* x : 깊이버퍼에 기록된 원근투영된 깊이값(NDC), y : 뷰스페이스 상의 z / farz 값 */
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFarZ;
    /* 픽셀의 월드 포지션을 구한다. */
    float4 vPixelWorld = GetWorldPositionFromDepth(In.vTexcoord, vDepthDesc.x, fViewZ);
    
    /* 1. 연산에 필요한 데이터들을 읽어들인다. */
    float3 vPixelToEye = normalize(g_vCamWorld - vPixelWorld).rgb;
    float3 vAlbedo = g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float3 vNormalWorld = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz * 2.0f - 1.0f; // 16 unorm을 변환하여 값 저장.
    float4 vORMHDesc = g_ORMHTexture.Sample(LinearSampler, In.vTexcoord);
    float fAO = vORMHDesc.r;
    float fRoughness = vORMHDesc.g;
    float fMetallic = vORMHDesc.b;
    float fHeight = vORMHDesc.a;
    
    /* 2. Point Light 연산 수행 */
    float3 vLightVector = c_DirectLight.vPosition - vPixelWorld.xyz;
    float fLightDist = length(vLightVector);
    vLightVector /= fLightDist;
    float3 vHalfway = normalize(vPixelToEye + vLightVector);
    
    float NdotI = max(0.0f, dot(vNormalWorld, vLightVector));
    float NdotH = max(0.0f, dot(vNormalWorld, vHalfway));
    float NdotO = max(0.0f, dot(vNormalWorld, vPixelToEye));
    
    float3 F0 = lerp(Fdielectric, vAlbedo, fMetallic); // fMetallic 값을 가지고 보간한다. FDielectric은 0.04f;
    float F = SchlickFresnel(F0, dot(vHalfway, vPixelToEye));
    float3 kd = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), fMetallic); // 물체의 확산반사계수 : 메탈릭 수치에 따라 보정되는 값임 >>> F로 반사되는 에너지를 제외한 에너지를 확산반사의 양으로 사용함.
    float3 vDiffuseBRDF = kd * vAlbedo;
    
    float D = NdfGGX(NdotH, fRoughness);
    float3 G = SchlickGGX(NdotI, NdotO, fRoughness);
    float3 vSpecularBRDf = (F * D * G) / max(1e-5f, 4.0f * NdotI * NdotO);

    float fAttenuation = pow(saturate((c_DirectLight.fFallOutEnd - fLightDist) / (c_DirectLight.fFallOutEnd - c_DirectLight.fFallOutStart)), 2.0f); //saturate((c_DirectLight.fFallOutEnd - fLightDist) / (c_DirectLight.fFallOutEnd - c_DirectLight.fFallOutStart));
    float3 vFinalRadiance = c_DirectLight.vRadiance * fAttenuation;
    
    
    float3 DirectLighting = ((vDiffuseBRDF * c_DirectLight.vDiffuse.rgb) + (vSpecularBRDf * c_DirectLight.vSpecular.rgb)) * vFinalRadiance * NdotI;
    
    // Shadow Factor 계산
    float fShadowFactor = 0.0f;
    
    Out.vColor = float4(DirectLighting.rgb * (1.0f - fShadowFactor), 1.0f);
    
    return Out;
}

PS_OUT PS_PBR_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    /* x : 깊이버퍼에 기록된 원근투영된 깊이값(NDC), y : 뷰스페이스 상의 z / farz 값 */
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_fFarZ;
    /* 픽셀의 월드 포지션을 구한다. */
    float4 vPixelWorld = GetWorldPositionFromDepth(In.vTexcoord, vDepthDesc.x, fViewZ);
    
    /* 1. 연산에 필요한 데이터들을 읽어들인다. */
    float3 vPixelToEye = normalize(g_vCamWorld - vPixelWorld).rgb;
    float3 vAlbedo = g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float3 vNormalWorld = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz * 2.0f - 1.0f; // 16 unorm을 변환하여 값 저장.
    float4 vORMHDesc = g_ORMHTexture.Sample(LinearSampler, In.vTexcoord);
    float fAO = vORMHDesc.r;
    float fRoughness = vORMHDesc.g;
    float fMetallic = vORMHDesc.b;
    float fHeight = vORMHDesc.a;
    
/* 2. Direction Light 연산 수행 */
    float3 vLightVector = normalize(c_DirectLight.vDirection) * -1.0f; // 전역 조명은 방향성이 이미 명확함
    //float fLightDist = length(vLightVector); // 조명의 길이 자체가 무의미.
    //vLightVector /= fLightDist;
    float3 vHalfway = normalize(vPixelToEye + vLightVector);
    
    float NdotI = max(0.0f, dot(vNormalWorld, vLightVector));
    float NdotH = max(0.0f, dot(vNormalWorld, vHalfway));
    float NdotO = max(0.0f, dot(vNormalWorld, vPixelToEye));
    
    float3 F0 = lerp(Fdielectric, vAlbedo, fMetallic); // fMetallic 값을 가지고 보간한다. FDielectric은 0.04f;
    float F = SchlickFresnel(F0, dot(vHalfway, vPixelToEye));
    float3 kd = lerp(float3(1.0f, 1.0f, 1.0f) - F, float3(0.0f, 0.0f, 0.0f), fMetallic); // 물체의 확산반사계수 : 메탈릭 수치에 따라 보정되는 값임 >>> F로 반사되는 에너지를 제외한 에너지를 확산반사의 양으로 사용함.
    float3 vDiffuseBRDF = kd * vAlbedo;
    
    float D = NdfGGX(NdotH, fRoughness);
    float3 G = SchlickGGX(NdotI, NdotO, fRoughness);
    float3 vSpecularBRDf = (F * D * G) / max(1e-5f, 4.0f * NdotI * NdotO);

    //float fAttenuation = pow(saturate((c_DirectLight.fFallOutEnd - fLightDist) / (c_DirectLight.fFallOutEnd - c_DirectLight.fFallOutStart)), 2.0f); //saturate((c_DirectLight.fFallOutEnd - fLightDist) / (c_DirectLight.fFallOutEnd - c_DirectLight.fFallOutStart));
    //float3 vFinalRadiance = ;
    
    float3 DirectLighting = ((vDiffuseBRDF * c_DirectLight.vDiffuse.rgb) + (vSpecularBRDf * c_DirectLight.vSpecular.rgb)) * c_DirectLight.vRadiance * NdotI;
    
    // Shadow Factor 계산
    float fShadowFactor = 0.0f;
    if (c_DirectLight.isShadow & 1)
    {

        // 1. 픽셀 월드좌표를 광원 기준 투영좌표로 이동 
        float4 vLightScreen = mul(vPixelWorld, g_LightViewMatrix);
        vLightScreen = mul(vLightScreen, g_LightProjMatrix);
        vLightScreen.xyz /= vLightScreen.w; // w 나누기 까지 수행.
        
        // 2. ndc 좌표를 texcoord 좌표롤 변환
        float2 vLightTexcoord = float2(vLightScreen.x, vLightScreen.y * -1.0f);
        vLightTexcoord += 1.0f;
        vLightTexcoord *= 0.5f;
        
        float fMaxBias = 0.3f;
        float fMinBias = 0.001f;
        float fSlopeScaledBias = max(fMaxBias * (1.0f - dot(vNormalWorld, vLightVector)), fMinBias);
        
        uint width, height, numMips;
        g_ShadowMapTexture.GetDimensions(0, width, height, numMips);
        
        // Texel Size
        float dx = 5.0 / (float) width;
        //float fLightDepth = g_ShadowMapTexture.Sample(LinearSampler, vLightTexcoord).r;
        fShadowFactor = PCF_Filter(vLightTexcoord.xy, vLightScreen.w - fSlopeScaledBias, dx, g_ShadowMapTexture);
        //float fLightDepth = g_ShadowMapTexture.SampleCmpLevelZero(ShadowCompareSampler, vLightTexcoord).r; // viewspace상의 z 값을 far로 나누어 저장했음.
        //if (fLightDepth + fSlopeScaledBias < vLightScreen.w)
        //    fShadowFactor = 0.0f;
    }

    
    
    
    
    Out.vColor = float4(DirectLighting.rgb * (1 - fShadowFactor), 1.0f);
    return Out;
}

PS_OUT PS_MAIN_LIGHTING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    /* x : 깊이버퍼에 기록된 원근투영된 깊이값(NDC), y : 뷰스페이스 상의 z / farz 값 */
    float4 vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord); 
    float fViewZ = vDepthDesc.y * g_fFarZ;
    /* 픽셀의 월드 포지션을 구한다. */
    float4 vPixelWorld = GetWorldPositionFromDepth(In.vTexcoord, vDepthDesc.x, fViewZ);
    
    /* 1. 연산에 필요한 데이터들을 읽어들인다. */
    float3 vPixelToEye = normalize(g_vCamWorld - vPixelWorld).rgb;
    float3 vAlbedo = g_AlbedoTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float3 vNormalWorld = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz * 2.0f - 1.0f; // 16 unorm을 변환하여 값 저장.
    float4 vORMHDesc = g_ORMHTexture.Sample(LinearSampler, In.vTexcoord);
    float fAO =         vORMHDesc.r;
    float fRoughness =  vORMHDesc.g;
    float fMetallic =   vORMHDesc.b;
    float fHeight =     vORMHDesc.a;

    /* 2. IBL 기반으로 AbientLighting 결과를 저장한다. */
    float3 vAmbientLighting = AmbientLightingByIBL(vAlbedo, vNormalWorld, vPixelToEye, fAO, fMetallic, fRoughness) * c_GlobalIBLVariable.fStrengthIBL;
    
    /* 3. 조명의 정보를 읽어들여 직접 조명에 대한 연산을 수행한다. */
    float3 vDirectLighting = g_DirectLightsTexture.Sample(LinearSampler, In.vTexcoord);

        
    /* 4. SSAO 는 간접광의 밝기를 조절하는. */
    //float SumSSAO = 0.0f;
    //[unroll]
    //for (int i = 0; i < 64; ++i)
    //{
    //    float fSSAO = Compute_SSAO(vPixelWorld.xyz, normalize(vNormalWorld), g_RandomTexcoords[i], 1.0f, g_DepthTexture, ssaoKernel[i]);
    //
    //    SumSSAO += fSSAO;
    //}
    //SumSSAO /= 64.f;
    //SumSSAO = 1.0f - SumSSAO;
    //vAmbientLighting *= pow(SumSSAO, 4);
    
    
    //Out.vColor = float4(SumSSAO, SumSSAO, SumSSAO /*+ vEmmision*/, 1.0f);
    Out.vColor = float4(vAmbientLighting + vDirectLighting /*+ vEmmision*/, 1.0f);
    Out.vColor = clamp(Out.vColor, 0.0f, c_GlobalIBLVariable.fHDRMaxLuminance);
    return Out;
}
PS_OUT PS_PBR_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float3 vLighting = g_LightingTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float3 vPBRBlur = g_SrcBlurTargetTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float2 vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).rg;
    float fViewZ = vDepth.y * g_fFarZ;
    float4 vPixelWorld = GetWorldPositionFromDepth(In.vTexcoord, vDepth.x, fViewZ); // 무조건 월드포지션 저장하는 타겟만들자. 세번이나 씀
    float fObjectDistance = length(vPixelWorld - g_vCamWorld);
    /* CoC 계산*/
    //float fCoc = (fObjectDistance - 5.0f) / 30.0f;
    float fCoc = c_DofVariable.fAperture *
             (c_DofVariable.fFocalLength * (fObjectDistance - c_DofVariable.fFocusDistance)) /
             (fObjectDistance * (c_DofVariable.fFocusDistance - (c_DofVariable.fFocalLength / g_fFarZ)));
    //fCoc = clamp(fCoc, 0.0, c_DofVariable.fDofBrightness);
    //vLighting = vLighting + vPBRBlur * fCoc;
    
    fCoc = clamp(fCoc, c_DofVariable.fBaseBlurPower, 1.0f);
    vPBRBlur *= (c_DofVariable.vBlurColor * c_DofVariable.fDofBrightness);
    vLighting = (vLighting * (1.0f - fCoc)) + (vPBRBlur * fCoc);
    
    Out.vColor = float4(vLighting, 1.0f); // PBR_BLURFINAL TARGET
    return Out;
}
PS_OUT PS_MAIN_COMBINE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    float3 vPostProcessing = g_PostProcessingTexture.Sample(LinearSampler, In.vTexcoord).rgb;
    float2 vDepth = g_DepthTexture.Sample(PointSampler, In.vTexcoord).rg;
    float fViewZ = vDepth.y * g_fFarZ;
    
    
    float3 vToneMapColor = float3(0.0f, 0.0f, 0.0f);
    if (TONE_FILMIC == c_GlobalIBLVariable.iToneMappingFlag)
    {
        vToneMapColor = FilmicToneMapping(vPostProcessing);
    }
    else if (TONE_UNCHARTED2 == c_GlobalIBLVariable.iToneMappingFlag)
    {
        vToneMapColor = Uncharted2ToneMapping(vPostProcessing);
    }
    else if (TONE_LUMAREINHARD == c_GlobalIBLVariable.iToneMappingFlag)
    {
        vToneMapColor = lumaBasedReinhardToneMapping(vPostProcessing);
    }
    else
    {
        vToneMapColor = LinearToneMapping(vPostProcessing);
    }

    
    
    
    // 플레이어가 물체보다 뒤에있는 경우, 특정 색상으로 그리기 
    float fPlayerDepthDesc = g_PlayerDepthTexture.Sample(LinearSampler, In.vTexcoord).r;
    float fPlayerViewZ = fPlayerDepthDesc;
    
    float3 vHideColor = g_vHideColor;
    float fViewZDiff = fPlayerViewZ - fViewZ - 1.0f;
    float fHideMin = 0.0f;
    float fHideMax = 10.0f;
    vHideColor = lerp(vToneMapColor, vHideColor, saturate(fViewZDiff / (fHideMax - fHideMin)));
    vToneMapColor = 0.0f < fViewZDiff ? vHideColor : vToneMapColor;
    
    vToneMapColor = lerp(vToneMapColor, vHideColor, (vDepth.y - fPlayerDepthDesc));
    Out.vColor = float4(vToneMapColor, 1.0f);
    return Out;
}

float2 g_TexelSize = { 1.0f / 400.f, 1.0f / 225.f };


PS_OUT PS_DOWNSAMPLE1(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = 0;
    
    vColor += g_BloomTexture.Sample(LinearSampler_Clamp, In.vTexcoord + g_TexelSize * float2(-0.5, -0.5));
    vColor += g_BloomTexture.Sample(LinearSampler_Clamp, In.vTexcoord + g_TexelSize * float2(0.5, -0.5));
    vColor += g_BloomTexture.Sample(LinearSampler_Clamp, In.vTexcoord + g_TexelSize * float2(-0.5, 0.5));
    vColor += g_BloomTexture.Sample(LinearSampler_Clamp, In.vTexcoord + g_TexelSize * float2(0.5, 0.5));

    Out.vColor = vColor * 0.25f;
    
    return Out;

}

PS_OUT PS_DOWNSAMPLE2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = 0;
    
    for (int y = -3; y <= 2; y++)
    {
        for (int x = -3; x <= 2; x++)
        {
            float2 fOffset = g_TexelSize * float2(x + 0.5f, y + 0.5f);
            vColor += g_BloomTexture.Sample(LinearSampler_Clamp, In.vTexcoord + fOffset);
        }
    }

    // 평균 계산 (총 36개 샘플을 사용했으므로 1/36을 곱함)
    Out.vColor = vColor * (1.0f / 36.0f);
    
    return Out;
}

float g_fWeights2[11] =
{
    0.0483939,
0.057938,
0.0666445,
0.0736536,
0.0782081,
0.079788,
0.0782081,
0.0736536,
0.0666445,
0.057938,
0.0483939,
};
float g_fDivide = 0.729464;

PS_OUT PS_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = 0.f;

    for (int i = -5; i < 5; i++)
    {
        for (int j = -5; j < 5; ++j)
        {
            vTexcoord = float2(In.vTexcoord.x, In.vTexcoord.y) + float2(g_TexelSize.x * i, g_TexelSize.y * j);

            Out.vColor += g_fWeights2[i + 5] * g_fWeights2[j + 5] * g_BloomTexture.Sample(LinearSampler_Clamp, vTexcoord);
        }
    }

    Out.vColor = Out.vColor / (g_fDivide * g_fDivide) * 3.f;

    return Out;
}



PS_OUT PS_AFTER_EFFECT(PS_IN In)
{   
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vDistortion = g_DistortionTexture.Sample(LinearSampler, In.vTexcoord);
    //vDistortion.x -= step(0.f, vDistortion.x) * 0.5f;
    //vDistortion.y -= step(0.f, vDistortion.y) * 0.5f;    
    
    //vector vFinal = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion  * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    vector vFinal = g_FinalTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    //vector vColor = g_EffectColorTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion  * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    vector vColor = g_EffectColorTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    vector vBloom1 = g_BloomTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    vector vBloom2 = g_BloomTexture2.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    
    vector vAccum = g_AccumulateTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f));
    float fRevealage = g_RevealageTexture.Sample(LinearSampler_Clamp, In.vTexcoord + vDistortion * float2(In.vTexcoord.x - 0.5f, In.vTexcoord.y - 0.5f)).r;
    
    vector vBloomColor = vBloom1 * 0.5f + vBloom2 * 0.2f;
    vector vParticle = float4(vAccum.rgb / max(0.001, vAccum.a), fRevealage);
    
    vFinal.rgb = vFinal.rgb * (1 - vColor.a) + vColor.rgb * vColor.a;
    vFinal.rgb = vFinal.rgb * fRevealage + vParticle.rgb * (1 - fRevealage);
    vFinal.rgb = vFinal.rgb + vBloomColor.rgb;

    
    vFinal.rgb *= c_DofVariable.fFadeRatio;
    Out.vColor = vFinal;
    
        
    return Out;
}

PS_OUT PS_AFTER_PARTICLE(PS_IN In)
{     
    PS_OUT Out = (PS_OUT) 0;
        
    float fRevealage = g_RevealageTexture.Sample(LinearSampler, In.vTexcoord).r;
    vector vBloom2 = g_BloomTexture2.Sample(LinearSampler, In.vTexcoord);

    float4 vBloomParticle = float4(vBloom2.rgb / max(0.001, vBloom2.a), fRevealage);
    
    Out.vColor = vBloomParticle;
        
    return Out;
}


// Physics Blur DownSample
PS_OUT PS_PBR_BLUR_DOWN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float x = In.vTexcoord.x;
    float y = In.vTexcoord.y;
    
    float dx = g_TexelSize.x;
    float dy = g_TexelSize.y;
    
    float3 a = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - 2 * dx, y + 2 * dy)).rgb;
    float3 b = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x, y + 2 * dy)).rgb;
    float3 c = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + 2 * dx, y + 2 * dy)).rgb;

    float3 d = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - 2 * dx, y)).rgb;
    float3 e = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x, y)).rgb;
    float3 f = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + 2 * dx, y)).rgb;

    float3 g = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - 2 * dx, y - 2 * dy)).rgb;
    float3 h = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x, y - 2 * dy)).rgb;
    float3 i = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + 2 * dx, y - 2 * dy)).rgb;

    float3 j = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - dx, y + dy)).rgb;
    float3 k = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + dx, y + dy)).rgb;
    float3 l = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - dx, y - dy)).rgb;
    float3 m = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + dx, y - dy)).rgb;

    float3 color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.125;
    
    Out.vColor = float4(color, 1.0f);

    return Out;
}
// Physics Blur UpSample
PS_OUT PS_PBR_BLUR_UP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float x = In.vTexcoord.x;
    float y = In.vTexcoord.y;
    
    float dx = g_TexelSize.x;
    float dy = g_TexelSize.y;
    
    float3 a = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - dx, y + dy)).rgb;
    float3 b = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x, y + dy)).rgb;
    float3 c = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + dx, y + dy)).rgb;

    float3 d = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - dx, y)).rgb;
    float3 e = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x, y)).rgb;
    float3 f = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + dx, y)).rgb;

    float3 g = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x - dx, y - dy)).rgb;
    float3 h = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x, y - dy)).rgb;
    float3 i = g_SrcBlurTargetTexture.Sample(LinearSampler_Clamp, float2(x + dx, y - dy)).rgb;

    float3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;

    Out.vColor = float4(color, 1.0f);

    return Out;
}


// Debug PixelShader 
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}




technique11 DefaultTechnique
{
    pass Debug // 0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0); // 깊이 버퍼 기본 옵션 사용 , 초기화 값 0.
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xfffffff); // Blend Option Default

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
        ComputeShader = NULL;

    }

    pass Lighting // 1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTING();
    }

    pass Combine // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_COMBINE();
    }

    pass AfterEffect // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_AFTER_EFFECT();
        ComputeShader = NULL;

    }

    pass AfterParticle // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_InvAlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_AFTER_PARTICLE();
        ComputeShader = NULL;

    }

    pass DOWNSAMPLE1 // 5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DOWNSAMPLE1();
        ComputeShader = NULL;

    }

    pass DOWNSAMPLE2 // 6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DOWNSAMPLE2();
        ComputeShader = NULL;

    }

    pass Blur // 7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLUR();
        ComputeShader = NULL;

    }

    pass PBR_Light_Point // 8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PBR_LIGHT_POINT();
    }

    pass PBR_Directional // 9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PBR_LIGHT_DIRECTIONAL();
    }

    pass PBR_BLUR_DOWN // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PBR_BLUR_DOWN();
        ComputeShader = NULL;

    }

    pass PBR_BLUR_UP // 10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PBR_BLUR_UP();
        ComputeShader = NULL;

    }

    pass PBR_Blur_Final // 11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_PBR_BLUR_FINAL();
    }
}