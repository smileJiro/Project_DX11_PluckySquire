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
	m_pTexture = _Textures[strSourceTexture];

	m_fPixelsPerUnrealUnit = jProperties["PixelsPerUnrealUnit"];

	json& jBakedRenderData = jProperties["BakedRenderData"];
	m_vSpriteStartUV = { 1,1 };
	m_vSpriteEndUV = { 0,0 };
	_float fMinX = D3D11_FLOAT32_MAX, fMinY = D3D11_FLOAT32_MAX;
	_float fMaxX = -D3D11_FLOAT32_MAX, fMaxY = -D3D11_FLOAT32_MAX;
	for (json& j : jBakedRenderData)
	{
		vBakedRenderDatas.push_back(_float4{ j["X"],j["Y"] ,j["Z"] ,j["W"] });
		m_vSpriteStartUV = _float2(min(vBakedRenderDatas.back().z, m_vSpriteStartUV.x), min(vBakedRenderDatas.back().w, m_vSpriteStartUV.y));
		m_vSpriteEndUV = _float2(max(vBakedRenderDatas.back().z, m_vSpriteEndUV.x), max(vBakedRenderDatas.back().w, m_vSpriteEndUV.y));
		fMinX = min(fMinX, vBakedRenderDatas.back().x);
		fMinY = min(fMinY, vBakedRenderDatas.back().y);
		fMaxX = max(fMaxX, vBakedRenderDatas.back().x);
		fMaxY = max(fMaxY, vBakedRenderDatas.back().y);
	}
	_float fWidth = abs(fMaxX - fMinX);
	_float fHeight = abs(fMaxY - fMinY);
	_float fXOffset = fMinX + fWidth / 2;
	_float fYOffset = fMinY + fHeight / 2;
	m_matSpriteTransform = XMMatrixScaling(fWidth, fHeight, 1);
	m_matSpriteTransform *= XMMatrixTranslation(fXOffset, fYOffset, 0);
	return S_OK;
}

HRESULT CToolSpriteFrame::Export(ofstream& _outfile)
{
	_outfile.write(reinterpret_cast<const char*>(&m_vSpriteStartUV), sizeof(_float2));
	_outfile.write(reinterpret_cast<const char*>(&m_vSpriteEndUV), sizeof(_float2));
	_outfile.write(reinterpret_cast<const char*>(&m_fPixelsPerUnrealUnit), sizeof(_float));
	_uint iCount = 0;
	string strTextureName = WstringToString(*m_pTexture->Get_SRVName(0));
	iCount = strTextureName.length();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	_outfile.write(strTextureName.c_str(), iCount);
	_outfile.write(reinterpret_cast<const char*>(&m_matSpriteTransform), sizeof(_matrix));
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
	m_strName = _jData["Name"];
	json& jProperties = _jData["Properties"];
	if (jProperties.contains("FramesPerSecond"))
		m_fFramesPerSecond = jProperties["FramesPerSecond"];

	json& jKeyFrames = jProperties["KeyFrames"];
	for (json& jKeyFrame : jKeyFrames)
	{
		_uint iFrameRun = jKeyFrame["FrameRun"];

		string strSpriteFileName = jKeyFrame["Sprite"]["ObjectName"];
		_uint iStart = (_uint)(strSpriteFileName.find_first_of('\'')) + 1;
		_uint iCount = (_uint)strSpriteFileName.find_last_of('\'') - iStart;
		strSpriteFileName = strSpriteFileName.substr(iStart, iCount);
		json& jSpriteFIle = _jPaperSprites[strSpriteFileName];
		m_SpriteFrames.push_back({ CToolSpriteFrame::Create(_pDevice, _pContext, jSpriteFIle, _Textures), iFrameRun });
	}
	m_iFrameCount = (_uint)m_SpriteFrames.size();
	return S_OK;
}

HRESULT CToolAnimation2D::Export(ofstream& _outfile)
{
	_uint iCount = 0;
	//Name
	iCount = m_strName.length();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	_outfile.write(m_strName.c_str(), iCount);
	//FramesPerSecond
	_outfile.write(reinterpret_cast<const char*>(&m_fFramesPerSecond), sizeof(_float));
	//FrameCount
	_outfile.write(reinterpret_cast<const char*>(&m_iFrameCount), sizeof(_uint));
	//Loop
	_outfile.write(reinterpret_cast<const char*>(&m_bLoop), sizeof(_bool));
	//SpeedMagnifier
	_outfile.write(reinterpret_cast<const char*>(&m_fSpeedMagnifier), sizeof(_float));
	
	//SpriteFrames
	iCount = m_SpriteFrames.size();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	for (auto& pairSpriteFrame : m_SpriteFrames)
	{
		static_cast<CToolSpriteFrame*>(pairSpriteFrame.first)->Export(_outfile);
		_outfile.write(reinterpret_cast<const char*>(&pairSpriteFrame.second), sizeof(_uint));
	}
    return S_OK;
}

void CToolAnimation2D::Set_CurrentFrame(_uint _iFrameIndex)
{
	m_iCurrentFrame = _iFrameIndex;
}



_uint CToolAnimation2D::Get_CurrentFrame()
{
	return m_iCurrentFrame;
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
