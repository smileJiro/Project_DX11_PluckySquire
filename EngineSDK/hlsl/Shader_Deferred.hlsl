#include "Engine_Shader_Define.hlsli"
#include "Engine_Shader_Function.hlsli"

// Object Matrix Data 
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInv, g_ProjMatrixInv;
float4x4 g_LightViewMatrix, g_LightProjMatrix;

// Deferred Shading RTV
Texture2D g_Texture, g_NormalTexture, g_DiffuseTexture, g_ShadeTexture, g_DepthTexture, g_SpecularTexture, g_LightDepthTexture, g_FinalTexture;

Texture2D g_EffectTexture, g_Effect_BrightnessTexture, g_Effect_Blur_XTeuxture, g_Effect_Blur_YTeuxture, g_Effect_DistortionTeuxture;

// Light Data
vector g_vLightDir, g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse, g_vLightAmbient, g_vLightSpecular;

// Default Mtrl
vector g_vMtrlAmbient = 1.0f, g_vMtrlSpecular = 1.0f; // 특별한 경우가 아니라면 재질의 Ambient와 Specular는 특정 값으로 고정 후, 조명의 값으로 변화를 준다. 

vector g_vCamPosition;
float g_fFarZ = 1000.f;

// Blur 
float g_dX = 0.0f;
float g_dY = 0.0f;
float g_fBloomWeight = 1.0f;
float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

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

// Debug PixelShader 
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    // 명암연산을 마친 후 LightAcc에 저장한다. 
    float4 vShade : SV_TARGET0; // 명암의 결과를 저장
    float4 vSpecular : SV_TARGET1; // Specular를 저장.
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vShade = saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient);

    Out.vShade = g_vLightDiffuse * saturate(vShade);

    vector vReflect = reflect(normalize(g_vLightDir), vNormal);

	/* 현재 픽셀의 월드위치가 필요하다. */
	/* 현재 픽셀 : 화면에 그려지고 있는 디퍼드 셰이딩을 요하는 객체들(지형, 프렐이어 몬스터)을 구성하는 픽셀들 <-> 직교투영으로 그리느느 사각형 버퍼의 픽셀(x)*/
	/* 뷰포트, 투영스페이스는 화면에 그려지는 영역이 고정되어있다. */
	/* 뷰포트 상 왼쪽 위에그려지는 픽셀의 텍스쿠드 (0, 0) == 투영스페이스 상의 왼쪽위 픽셀의 위치(-1.f, 1.f) */
	/* 이 연산을 통해 투영스페이스 상의 위치를 구하낟.*/

    vector vPosition = 0.f;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1.f / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vPosition *= fViewZ;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 */
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 로컬정점위치 * 월드행렬 */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLook = vPosition - g_vCamPosition;

    float fSpecular = pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* 현재 픽셀의 월드위치가 필요하다. */
	/* 현재 픽셀 : 화면에 그려지고 있는 디퍼드 셰이딩을 요하는 객체들(지형, 프렐이어 몬스터)을 구성하는 픽셀들 <-> 직교투영으로 그리느느 사각형 버퍼의 픽셀(x)*/
	/* 뷰포트, 투영스페이스는 화면에 그려지는 영역이 고정되어있다. */
	/* 뷰포트 상 왼쪽 위에그려지는 픽셀의 텍스쿠드 (0, 0) == 투영스페이스 상의 왼쪽위 픽셀의 위치(-1.f, 1.f) */
	/* 이 연산을 통해 투영스페이스 상의 위치를 구하낟.*/

    vector vPosition = 0.f;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1.f / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vPosition *= fViewZ;

	/* 로컬정점위치 * 월드행렬 * 뷰행렬 */
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 로컬정점위치 * 월드행렬 */
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;

    float fDistance = length(vLightDir);

	/* 빛의 감쇄값*/
	/* 픽셀과 빛이 가까우면 1에 가까운값으로 멀면 0에 가까운 값으로 범위를 벗어나면 다 0으로. */
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    vector vShade = fAtt * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient));

    Out.vShade = g_vLightDiffuse * saturate(vShade);

    vector vReflect = reflect(normalize(vLightDir), vNormal);

    vector vLook = vPosition - g_vCamPosition;


    float fSpecular = fAtt * pow(saturate(dot(normalize(vLook) * -1.f, normalize(vReflect))), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);

    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;
    
    return Out;
}

struct PS_OUT_BLUR
{
    vector vColor : SV_TARGET0;
};

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
    }

    pass Light_Directional // 1
    {
        SetRasterizerState(RS_Default);
        // 깊이버퍼를 기록하지 않겠다. >>> 깊이버퍼를 기록해버리면 기존 object들의 깊이값이 모두 의미가 없어짐. 직교투영 사각형으로 그리다보니 0 값으로 다 채워질 거임.
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL(); // 방향성 조명에 대한 shade 값을 계산하는 pixelshader 
    }

    pass Light_Point // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Final // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

}