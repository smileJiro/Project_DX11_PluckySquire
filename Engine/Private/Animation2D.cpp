#include "Animation2D.h"
#include "GameInstance.h"

//1  0
//2 4
CSpriteFrame::CSpriteFrame()
{
}
CSpriteFrame::CSpriteFrame(const CSpriteFrame& _Prototype)
	: m_vSpriteStartUV(_Prototype.m_vSpriteStartUV)
	, m_vSpriteEndUV(_Prototype.m_vSpriteEndUV)
	, m_pTexture(_Prototype.m_pTexture)
	, m_fPixelsPerUnrealUnit(_Prototype.m_fPixelsPerUnrealUnit)
	, m_matSpriteTransform(_Prototype.m_matSpriteTransform)
{

}
CSpriteFrame::~CSpriteFrame()
{

}
HRESULT CSpriteFrame::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath,ifstream& _inFile, map<string, CTexture*>& _Textures)
{
	//StartUV
	_inFile.read(reinterpret_cast<char*>(&m_vSpriteStartUV), sizeof(_float2));
	//EndUV
	_inFile.read(reinterpret_cast<char*>(&m_vSpriteEndUV), sizeof(_float2));
	//PixelsPerUnrealUnit
	_inFile.read(reinterpret_cast<char*>(&m_fPixelsPerUnrealUnit), sizeof(_float));
	//Texture
	_uint iCount = 0;
	_inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	_char szTextureName[MAX_PATH] = "";
	_inFile.read(szTextureName, iCount);
	szTextureName[iCount] = '\0';
	auto& pairTexture = _Textures.find(szTextureName);
	if (pairTexture == _Textures.end())
		return E_FAIL;
	m_pTexture = pairTexture->second;
	if (nullptr == m_pTexture)
		return E_FAIL;
	_inFile.read(reinterpret_cast<char*>(&m_matSpriteTransform), sizeof(_matrix));
	return S_OK;
}
HRESULT CSpriteFrame::Bind_ShaderResource(CShader* _pShader)
{
	if (FAILED(m_pTexture->Bind_ShaderResource(_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_vSpriteStartUV", &m_vSpriteStartUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_vSpriteEndUV", &m_vSpriteEndUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(_pShader->Bind_RawValue("g_fPixelsPerUnrealUnit", &m_fPixelsPerUnrealUnit, sizeof(_float))))
		return E_FAIL;
	return S_OK;
}

CSpriteFrame* CSpriteFrame::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures)
{
	CSpriteFrame* pInstance = new CSpriteFrame();

	if (FAILED(pInstance->Initialize(_pDevice, _pContext, szDirPath,_infIle,_Textures)))
	{
		MSG_BOX("SpriteFrame Create Failed");
		Safe_Release(pInstance);
	}

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

//==============================Animation2D==============================

CAnimation2D::CAnimation2D()
	:CAnimation()
{
}

CAnimation2D::CAnimation2D(const CAnimation2D& _Prototype)
	:CAnimation(_Prototype)
	, m_strName(_Prototype.m_strName)
	, m_fFramesPerSecond(_Prototype.m_fFramesPerSecond)
	, m_iFrameCount(_Prototype.m_iFrameCount)
{
	for (auto& pSpriteFrame : _Prototype.m_SpriteFrames)
	{
		m_SpriteFrames.push_back({ pSpriteFrame.first->Clone(),pSpriteFrame.second });
	}
}

HRESULT CAnimation2D::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures)
{
	_uint iCount = 0;
	//Name
	_infIle.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	m_strName.resize(iCount);
	_infIle.read(const_cast<char*>(m_strName.c_str()), iCount);
	//FramesPerSecond
	_infIle.read(reinterpret_cast<char*>(&m_fFramesPerSecond), sizeof(_float));
	//FrameCount
	_infIle.read(reinterpret_cast<char*>(&m_iFrameCount), sizeof(_uint));
	//Loop
	_infIle.read(reinterpret_cast<char*>(&m_bLoop), sizeof(_bool));
	//SpeedMagnifier
	_infIle.read(reinterpret_cast<char*>(&m_fSpeedMagnifier), sizeof(_float));
	//SpriteFrames
	_infIle.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	m_SpriteFrames.reserve(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		CSpriteFrame* pSpriteFrame = CSpriteFrame::Create(_pDevice, _pContext, szDirPath,_infIle, _Textures);
		if (nullptr == pSpriteFrame)
			return E_FAIL;
		_uint iFrameRun = 0;
		_infIle.read(reinterpret_cast<char*>(&iFrameRun), sizeof(_uint));
		m_SpriteFrames.push_back({ pSpriteFrame,iFrameRun });
	}

	return S_OK;
}



HRESULT CAnimation2D::Bind_ShaderResource(CShader* _pShader)
{
	if (FAILED(m_SpriteFrames[m_iCurrentFrame].first->Bind_ShaderResource(_pShader)))
		return E_FAIL;


	return S_OK;
}

_bool CAnimation2D::Play_Animation(_float _fTimeDelta)
{
	if (m_iCurrentFrame >= m_iFrameCount -1 )
	{
		if (m_bLoop)
		{
			Reset();
		}
		else
		{
			return true;
		}
	}

	if (m_fCurrentFrameTime >= 1 / m_fFramesPerSecond)
	{
		m_fCurrentFrameTime = 0;
		m_iCurrentSubFrame++;
		if (m_iCurrentSubFrame >= m_SpriteFrames[m_iCurrentFrame].second)
		{
			m_iCurrentSubFrame = 0;
			m_iCurrentFrame++;
		}
	}
	if (m_iCurrentFrame >= m_iFrameCount)
		m_iCurrentFrame = m_iFrameCount -1;
		
	m_fCurrentFrameTime += _fTimeDelta * m_fSpeedMagnifier;
	return false;
}

void CAnimation2D::Reset()
{
	__super::Reset();
}

void CAnimation2D::Add_SpriteFrame(CSpriteFrame* _pSpriteFrame, _uint _iFrameRun)
{
	m_SpriteFrames.push_back({ _pSpriteFrame ,_iFrameRun});
}

const _matrix* CAnimation2D::Get_CurrentSpriteTransform() 
{
	return GetCurrentSprite()->Get_Transform();
}

_float CAnimation2D::Get_Progress()
{
	_float fProgerss = 0;
	if (m_iFrameCount > 0)
	{
		fProgerss = (_float)(Get_AccumulativeSubFrameCount(m_iCurrentFrame) + m_iCurrentSubFrame) / (_float)Get_AccumulativeSubFrameCount(m_iFrameCount - 1);
	}
	return fProgerss;
}

void CAnimation2D::Set_Progress(_float _fProgerss)
{
	_int iTotalSubFrameCount = (_int)Get_AccumulativeSubFrameCount(m_iFrameCount - 1);
	_int iTargetSubFrame = (_int)(_fProgerss * iTotalSubFrameCount);
	_float fRemainTime = m_fFramesPerSecond * iTotalSubFrameCount * _fProgerss - (m_fFramesPerSecond * iTargetSubFrame);
	m_fCurrentFrameTime = fRemainTime;
	for (_uint i = 0; i < m_iFrameCount; i++)
	{
		_int iFrameRun = (_int)m_SpriteFrames[i].second;
		if (iTargetSubFrame - iFrameRun < 0)
		{
			m_iCurrentFrame = i;
			m_iCurrentSubFrame = iTargetSubFrame;
			return;
		}
		iTargetSubFrame -= iFrameRun;
	}
}

_uint CAnimation2D::Get_AccumulativeSubFrameCount(_uint _iFrameIndex)
{
	_uint iAccumulativeSubFrames = 0;
	for (_uint i = 0; i < _iFrameIndex; i++)
	{
		iAccumulativeSubFrames += m_SpriteFrames[i].second;
	}
	return iAccumulativeSubFrames;
}


CAnimation2D* CAnimation2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures)
{
	CAnimation2D* pInstance = new CAnimation2D();

	if (FAILED(pInstance->Initialize(_pDevice, _pContext, szDirPath,_infIle,_Textures)))
	{
		MSG_BOX("Animation2D Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation2D* CAnimation2D::Clone()
{
	return new CAnimation2D(*this);
}

void CAnimation2D::Free()
{
	for (auto& pSpriteFrame : m_SpriteFrames)
	{
		Safe_Release(pSpriteFrame.first);
	}
	__super::Free();
}
