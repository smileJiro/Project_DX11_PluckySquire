#include "stdafx.h"
#include "ToolAnimation2D.h"


CToolSpriteFrame::CToolSpriteFrame(const CToolSpriteFrame& _Prototype)
	:CSpriteFrame(_Prototype)
{
}

CToolSpriteFrame::~CToolSpriteFrame()
{
}
HRESULT CToolSpriteFrame::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, CTexture*>& _Textures)
{
	json& jProperties = _jData["Properties"];

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
		vBakedRenderDatas.push_back(_float4{ j["X"],j["Y"] ,j["Z"] ,j["W"] });
		vSpriteStartUV = _float2(min(vBakedRenderDatas.back().z, vSpriteStartUV.x), min(vBakedRenderDatas.back().w, vSpriteStartUV.y));
		vSpriteEndUV = _float2(max(vBakedRenderDatas.back().z, vSpriteEndUV.x), max(vBakedRenderDatas.back().w, vSpriteEndUV.y));
		fMinX = min(fMinX, vBakedRenderDatas.back().x);
		fMinY = min(fMinY, vBakedRenderDatas.back().y);
		fMaxX = max(fMaxX, vBakedRenderDatas.back().x);
		fMaxY = max(fMaxY, vBakedRenderDatas.back().y);
	}
	_float fWidth = abs(fMaxX - fMinX);
	_float fHeight = abs(fMaxY - fMinY);
	_float fXOffset = fMinX + fWidth / 2;
	_float fYOffset = fMinY + fHeight / 2;
	matSpriteTransform = XMMatrixScaling(fWidth, fHeight, 1);
	matSpriteTransform *= XMMatrixTranslation(fXOffset, fYOffset, 0);
	return S_OK;
}

HRESULT CToolSpriteFrame::Export(ofstream& _outfile)
{
	_outfile.write(reinterpret_cast<const char*>(&vSpriteStartUV), sizeof(_float2));
	_outfile.write(reinterpret_cast<const char*>(&vSpriteEndUV), sizeof(_float2));
	_outfile.write(reinterpret_cast<const char*>(&fPixelsPerUnrealUnit), sizeof(_float));
	_uint iCount = 0;
	string strTextureName = WstringToString(*pTexture->Get_SRVName(0));
	iCount = strTextureName.length();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	_outfile.write(strTextureName.c_str(), iCount);
	_outfile.write(reinterpret_cast<const char*>(&matSpriteTransform), sizeof(_matrix));
	return S_OK;
}

CToolSpriteFrame* CToolSpriteFrame::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, CTexture*>& _Textures)
{
	CToolSpriteFrame* pInstance = new CToolSpriteFrame();

	if (FAILED(pInstance->Initialize(_pDevice, _pContext, _jData, _Textures)))
	{
		MSG_BOX("SpriteFrame Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CToolSpriteFrame* CToolSpriteFrame::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures)
{
	CToolSpriteFrame* pInstance = new CToolSpriteFrame();

	if (FAILED(static_cast<CSpriteFrame*>(pInstance)->Initialize(_pDevice, _pContext,_szDirPath, _infIle,_Textures)))
	{
		MSG_BOX("SpriteFrame Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}



CToolSpriteFrame* CToolSpriteFrame::Clone()
{
	return new CToolSpriteFrame(*this);
}

void CToolSpriteFrame::Free()
{
	__super::Free();
}

//===========================================ANIMATION2D===========================================

CToolAnimation2D::CToolAnimation2D(const CToolAnimation2D& _Prototype)
	:CAnimation2D(_Prototype)
{
}


HRESULT CToolAnimation2D::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures)
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
		SpriteFrames.push_back({ CToolSpriteFrame::Create(_pDevice, _pContext, jSpriteFIle, _Textures), iFrameRun });
	}
	iFrameCount = (_uint)SpriteFrames.size();
	return S_OK;
}

HRESULT CToolAnimation2D::Export(ofstream& _outfile)
{
	_uint iCount = 0;
	//Name
	iCount = strName.length();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	_outfile.write(strName.c_str(), iCount);
	//FramesPerSecond
	_outfile.write(reinterpret_cast<const char*>(&fFramesPerSecond), sizeof(_float));
	//FrameCount
	_outfile.write(reinterpret_cast<const char*>(&iFrameCount), sizeof(_uint));
	//Loop
	_outfile.write(reinterpret_cast<const char*>(&bLoop), sizeof(_bool));
	//SpriteFrames
	iCount = SpriteFrames.size();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	for (auto& pairSpriteFrame : SpriteFrames)
	{
		static_cast<CToolSpriteFrame*>(pairSpriteFrame.first)->Export(_outfile);
		_outfile.write(reinterpret_cast<const char*>(&pairSpriteFrame.second), sizeof(_uint));
	}
    return S_OK;
}

void CToolAnimation2D::Set_CurrentFrame(_uint _iFrameIndex)
{
	iCurrentFrame = _iFrameIndex;
}

void CToolAnimation2D::Set_Progerss(_float _fProgerss)
{
	_int iTotalSubFrameCount = (_int)Get_AccumulativeSubFrameCount(iFrameCount - 1);
	_int iTargetSubFrame = (_int)(_fProgerss * iTotalSubFrameCount);
	for (_uint i = 0; i< iFrameCount ; i++)
	{
		_int iFrameRun = (_int)SpriteFrames[i].second;
		if (iTargetSubFrame- iFrameRun < 0)
		{
			iCurrentFrame = i;
			iCurrentSubFrame = iTargetSubFrame;
			return;
		}
		iTargetSubFrame -= iFrameRun;
	}
}

_uint CToolAnimation2D::Get_CurrentFrame()
{
	return iCurrentFrame;
}

_float CToolAnimation2D::Get_Progerss()
{
	_float fProgerss = 0;
	if (iFrameCount > 0)
	{
		fProgerss = (_float)(Get_AccumulativeSubFrameCount(iCurrentFrame) + iCurrentSubFrame) / (_float)Get_AccumulativeSubFrameCount(iFrameCount - 1);
	}
	return fProgerss;
}

_uint CToolAnimation2D::Get_AccumulativeSubFrameCount(_uint _iFrameIndex)
{
	_uint iAccumulativeSubFrames = 0;
	for (_uint i = 0; i < _iFrameIndex; i++)
	{
		iAccumulativeSubFrames += SpriteFrames[i].second;
	}
	return iAccumulativeSubFrames;
}



CToolAnimation2D* CToolAnimation2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json& _jData, map<string, json>& _jPaperSprites, map<string, CTexture*>& _Textures)
{
	CToolAnimation2D* pInstance = new CToolAnimation2D();

	if (FAILED(pInstance->Initialize(_pDevice, _pContext, _jData, _jPaperSprites, _Textures)))
	{
		MSG_BOX("Animation2D Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CToolAnimation2D* CToolAnimation2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures)
{
	CToolAnimation2D* pInstance = new CToolAnimation2D();

	if (FAILED(static_cast<CAnimation2D*>(pInstance)->Initialize(_pDevice, _pContext, _szDirPath, _infIle, _Textures)))
	{
		MSG_BOX("Animation2D Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CToolAnimation2D* CToolAnimation2D::Clone()
{
	return new CToolAnimation2D(*this);
}

void CToolAnimation2D::Free()
{
	__super::Free();
}
