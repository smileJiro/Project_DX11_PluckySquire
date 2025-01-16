#include "Animation2D.h"
#include "GameInstance.h"

//1  0
//2 4

CSpriteFrame::CSpriteFrame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string strDirectory, json& jData)
{
	json& jSprite = jData["Sprite"];
	string objName = jSprite["ObjectName"];
	_uint iStart = objName.find_first_of('\'') + 1;
	_uint iCount = objName.find_last_of('\'') - iStart;
	objName = objName.substr(iStart, iCount);
	objName = strDirectory +"Frames/" + objName + ".json";
	json jSpriteObject;
	std::ifstream input_file(objName.c_str());
	if (!input_file.is_open())
		return ;
	input_file >> jSpriteObject;
	input_file.close();

	json& jProperties = jSpriteObject.front() ["Properties"];

	json& jSpriteStart = jProperties["BakedSourceUV"];

	json& jSpriteSize = jProperties["BakedSourceDimension"];


	string strSourceTexture = jProperties["BakedSourceTexture"]["ObjectName"];
	iStart = strSourceTexture.find_first_of('\'') + 1;
	iCount = strSourceTexture.find_last_of('\'') - iStart;
	strSourceTexture = strSourceTexture.substr(iStart, iCount);
	strSourceTexture += ".png";
	strSourceTexture = strDirectory + "Textures/" + strSourceTexture;


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	wstring wstrSourceTexture = pGameInstance->StringToWString(strSourceTexture);
	pTexture = CTexture::Create(_pDevice,_pContext, wstrSourceTexture.c_str());
	fPixelsPerUnrealUnit = jProperties["PixelsPerUnrealUnit"];
	json& jBakedRenderData = jProperties["BakedRenderData"];
	vSpriteStartUV = { 1,1 };
	vSpriteEndUV = { 0,0 };
	for (json& j : jBakedRenderData)
	{
		fBakedRenderData.push_back(_float4{j["X"],j["Y"] ,j["Z"] ,j["W"] });
		vSpriteStartUV = _float2(min(fBakedRenderData.back().z, vSpriteStartUV.x), min(fBakedRenderData.back().w, vSpriteStartUV.y));
		vSpriteEndUV = _float2(max(fBakedRenderData.back().z, vSpriteEndUV.x), max(fBakedRenderData.back().w, vSpriteEndUV.y));
	}

	iFrameRun = jData["FrameRun"];
}
CSpriteFrame::CSpriteFrame(const CSpriteFrame& _Prototype)
	: vSpriteStartUV(_Prototype.vSpriteStartUV)
	, vSpriteEndUV(_Prototype.vSpriteEndUV)
	, pTexture(_Prototype.pTexture)
	, fPixelsPerUnrealUnit(_Prototype.fPixelsPerUnrealUnit)
	, fBakedRenderData(_Prototype.fBakedRenderData)
	, iFrameRun(_Prototype.iFrameRun)
{

}
CSpriteFrame::~CSpriteFrame()
{

}
HRESULT CSpriteFrame::Bind_ShaderResource(CShader* _pShader)
{
	if (FAILED(pTexture->Bind_ShaderResource(_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_vSpriteStartUV", &vSpriteStartUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_vSpriteEndUV", &vSpriteEndUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_fPixelsPerUnrealUnit", &fPixelsPerUnrealUnit, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}
CSpriteFrame* CSpriteFrame::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData)
{
	CSpriteFrame* pInstance = new CSpriteFrame(_pDevice, _pContext, _strDirectory, _jData);

	return pInstance;
}
CSpriteFrame* CSpriteFrame::Clone()
{
	return new CSpriteFrame(*this);
}
void CSpriteFrame::Free()
{
	Safe_Release(pTexture);
	__super::Free();
}
;

CAnimation2D::CAnimation2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData)
{
	strName = _jData["Name"];
	json& jProperties = _jData["Properties"];
	if (jProperties.contains("FramesPerSecond"))
		fFramesPerSecond = jProperties["FramesPerSecond"];
	json& jKeyFrames = jProperties["KeyFrames"];
	for (json& jKeyFrame : jKeyFrames)
	{

		SpriteFrames.push_back(CSpriteFrame::Create(_pDevice, _pContext,_strDirectory, jKeyFrame));
	}
	iFrameCount = SpriteFrames.size();

}

CAnimation2D::CAnimation2D(const CAnimation2D& _Prototype)
	:strName(_Prototype.strName)
	, fFramesPerSecond(_Prototype.fFramesPerSecond)
	, iFrameCount(_Prototype.iFrameCount)
	, bLoop(_Prototype.bLoop)
{
	for (auto& pSpriteFrame : _Prototype.SpriteFrames)
	{
		SpriteFrames.push_back(pSpriteFrame->Clone());
	}
}

HRESULT CAnimation2D::Bind_ShaderResource(CShader* _pShader)
{
	if (FAILED(SpriteFrames[iCurrentFrame]->Bind_ShaderResource(_pShader)))
		return E_FAIL;


	return S_OK;
}

_bool CAnimation2D::Play_Animation(_float _fTimeDelta)
{
	if (iCurrentFrame >= iFrameCount -1 )
	{
		if (bLoop)
		{
			iCurrentFrame = 0;
			iCurrentSubFrame = 0;
		}
		else
		{
			return true;
		}
	}

	if (fCurrentFrameTime >= 1 / fFramesPerSecond)
	{
		fCurrentFrameTime = 0;
		iCurrentSubFrame++;
		if (iCurrentSubFrame >= SpriteFrames[iCurrentFrame]->Get_FrameRun())
		{
			iCurrentSubFrame = 0;
			iCurrentFrame++;
		}
	}
	if (iCurrentFrame > iFrameCount)
		iCurrentFrame = iFrameCount -1;
		
	fCurrentFrameTime += _fTimeDelta;
	return false;
}



CAnimation2D* CAnimation2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, string _strDirectory, json& _jData)
{
	CAnimation2D* pInstance = new CAnimation2D(_pDevice, _pContext, _strDirectory, _jData);

	return pInstance;
}

CAnimation2D* CAnimation2D::Clone()
{
	return new CAnimation2D(*this);
}

void CAnimation2D::Free()
{
	for (auto& pSpriteFrame : SpriteFrames)
	{
		Safe_Release(pSpriteFrame);
	}
	__super::Free();
}
