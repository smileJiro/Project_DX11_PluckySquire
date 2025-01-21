#include "Animation2D.h"
#include "GameInstance.h"

//1  0
//2 4

CSpriteFrame::CSpriteFrame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,json& jData,  map<string, CTexture*>& _Textures)
{
	json& jProperties = jData["Properties"];

	json& jSpriteStart = jProperties["BakedSourceUV"];

	json& jSpriteSize = jProperties["BakedSourceDimension"];

	_uint iStart;
	_uint iCount;
	json& jBakedSourceTexture = jProperties["BakedSourceTexture"];
	string strSourceTexture = jBakedSourceTexture["ObjectName"];
	iStart = (_uint)(strSourceTexture.find_first_of('\'')) + 1;
	iCount = (_uint)strSourceTexture.find_last_of('\'') - iStart;
	strSourceTexture = strSourceTexture.substr(iStart, iCount);
	pTexture = _Textures[strSourceTexture];

	fPixelsPerUnrealUnit = jProperties["PixelsPerUnrealUnit"];

	json& jBakedRenderData = jProperties["BakedRenderData"];
	vSpriteStartUV = { 1,1 };
	vSpriteEndUV = { 0,0 };
	_float fMinX = D3D11_FLOAT32_MAX, fMinY = D3D11_FLOAT32_MAX;
	_float fMaxX = -D3D11_FLOAT32_MAX, fMaxY = -D3D11_FLOAT32_MAX;
	for (json& j : jBakedRenderData)
	{
		fBakedRenderData.push_back(_float4{j["X"],j["Y"] ,j["Z"] ,j["W"] });
		vSpriteStartUV = _float2(min(fBakedRenderData.back().z, vSpriteStartUV.x), min(fBakedRenderData.back().w, vSpriteStartUV.y));
		vSpriteEndUV = _float2(max(fBakedRenderData.back().z, vSpriteEndUV.x), max(fBakedRenderData.back().w, vSpriteEndUV.y));
		fMinX = min(fMinX, fBakedRenderData.back().x);
		fMinY = min(fMinY, fBakedRenderData.back().y);
		fMaxX = max(fMaxX, fBakedRenderData.back().x);
		fMaxY = max(fMaxY, fBakedRenderData.back().y);
	}
	_float fWidth= abs(fMaxX - fMinX);
	_float fHeight = abs(fMaxY - fMinY);
	_float fXOffset = fMinX + fWidth / 2;
	_float fYOffset = fMinY + fHeight / 2;
	matSpriteTransform = XMMatrixScaling(fWidth, fHeight, 1);
	matSpriteTransform *= XMMatrixTranslation(fXOffset, fYOffset, 0);
}
CSpriteFrame::CSpriteFrame(const CSpriteFrame& _Prototype)
	: vSpriteStartUV(_Prototype.vSpriteStartUV)
	, vSpriteEndUV(_Prototype.vSpriteEndUV)
	, pTexture(_Prototype.pTexture)
	, fPixelsPerUnrealUnit(_Prototype.fPixelsPerUnrealUnit)
	, fBakedRenderData(_Prototype.fBakedRenderData)
	, matSpriteTransform(_Prototype.matSpriteTransform)
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
CSpriteFrame* CSpriteFrame::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, CTexture*>& _Textures)
{
	CSpriteFrame* pInstance = new CSpriteFrame(_pDevice, _pContext, _jData, _Textures);

	return pInstance;
}
CSpriteFrame* CSpriteFrame::Clone()
{
	return new CSpriteFrame(*this);
}
void CSpriteFrame::Free()
{
	__super::Free();
}
;

CAnimation2D::CAnimation2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures)
{
	strName = _jData["Name"];
	json& jProperties = _jData["Properties"];
	if (jProperties.contains("FramesPerSecond"))
		fFramesPerSecond = jProperties["FramesPerSecond"];

	json& jKeyFrames = jProperties["KeyFrames"];
	for (json& jKeyFrame : jKeyFrames)
	{

		_uint iFrameRun = jKeyFrame["FrameRun"];

		string strSpriteFileName = jKeyFrame["Sprite"]["ObjectName"];
		_uint iStart = (_uint)(strSpriteFileName.find_first_of('\'')) + 1;
		_uint iCount = (_uint)strSpriteFileName.find_last_of('\'') - iStart;
		strSpriteFileName = strSpriteFileName.substr(iStart, iCount);
		json& jSpriteFIle = _jPaperSprites[strSpriteFileName];
		SpriteFrames.push_back({ CSpriteFrame::Create(_pDevice, _pContext, jSpriteFIle, _Textures), iFrameRun});
	}
	iFrameCount = (_uint)SpriteFrames.size();

}

CAnimation2D::CAnimation2D(const CAnimation2D& _Prototype)
	:strName(_Prototype.strName)
	, fFramesPerSecond(_Prototype.fFramesPerSecond)
	, iFrameCount(_Prototype.iFrameCount)
	, bLoop(_Prototype.bLoop)
{
	for (auto& pSpriteFrame : _Prototype.SpriteFrames)
	{
		SpriteFrames.push_back({ pSpriteFrame.first->Clone(),pSpriteFrame.second });
	}
}

HRESULT CAnimation2D::Bind_ShaderResource(CShader* _pShader)
{
	if (FAILED(SpriteFrames[iCurrentFrame].first->Bind_ShaderResource(_pShader)))
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
		if (iCurrentSubFrame >= SpriteFrames[iCurrentFrame].second)
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

void CAnimation2D::Add_SpriteFrame(CSpriteFrame* _pSpriteFrame, _uint _iFrameRun)
{
	SpriteFrames.push_back({ _pSpriteFrame ,_iFrameRun});
}

void CAnimation2D::Reset_CurrentTrackPosition()
{
	iCurrentFrame = 0;
	iCurrentSubFrame = 0;
	fCurrentFrameTime = 0;
}

const _matrix* CAnimation2D::Get_CurrentSpriteTransform() 
{
	return GetCurrentSprite()->Get_Transform();
}




CAnimation2D* CAnimation2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures)
{
	CAnimation2D* pInstance = new CAnimation2D(_pDevice, _pContext, _jData, _jPaperSprites, _Textures);

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
		Safe_Release(pSpriteFrame.first);
	}
	__super::Free();
}
