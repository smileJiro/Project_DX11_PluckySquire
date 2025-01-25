#include "Animation2D.h"
#include "GameInstance.h"

//1  0
//2 4
CSpriteFrame::CSpriteFrame()
{
}
CSpriteFrame::CSpriteFrame(const CSpriteFrame& _Prototype)
	: vSpriteStartUV(_Prototype.vSpriteStartUV)
	, vSpriteEndUV(_Prototype.vSpriteEndUV)
	, pTexture(_Prototype.pTexture)
	, fPixelsPerUnrealUnit(_Prototype.fPixelsPerUnrealUnit)
	, matSpriteTransform(_Prototype.matSpriteTransform)
{

}
CSpriteFrame::~CSpriteFrame()
{

}
HRESULT CSpriteFrame::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath,ifstream& _inFile, map<string, CTexture*>& _Textures)
{
	//StartUV
	_inFile.read(reinterpret_cast<char*>(&vSpriteStartUV), sizeof(_float2));
	//EndUV
	_inFile.read(reinterpret_cast<char*>(&vSpriteEndUV), sizeof(_float2));
	//PixelsPerUnrealUnit
	_inFile.read(reinterpret_cast<char*>(&fPixelsPerUnrealUnit), sizeof(_float));
	//Texture
	_uint iCount = 0;
	_inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	_char szTextureName[MAX_PATH] = "";
	_inFile.read(szTextureName, iCount);
	szTextureName[iCount] = '\0';
	auto& pairTexture = _Textures.find(szTextureName);
	if (pairTexture == _Textures.end())
		return E_FAIL;
	pTexture = pairTexture->second;
	if (nullptr == pTexture)
		return E_FAIL;
	_inFile.read(reinterpret_cast<char*>(&matSpriteTransform), sizeof(_matrix));
	return S_OK;
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
{
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

HRESULT CAnimation2D::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* szDirPath, ifstream& _infIle, map<string, CTexture*>& _Textures)
{
	_uint iCount = 0;
	//Name
	_infIle.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	strName.resize(iCount);
	_infIle.read(const_cast<char*>(strName.c_str()), iCount);
	//FramesPerSecond
	_infIle.read(reinterpret_cast<char*>(&fFramesPerSecond), sizeof(_float));
	//FrameCount
	_infIle.read(reinterpret_cast<char*>(&iFrameCount), sizeof(_uint));
	//Loop
	_infIle.read(reinterpret_cast<char*>(&bLoop), sizeof(_bool));
	//SpriteFrames
	_infIle.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	SpriteFrames.reserve(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		CSpriteFrame* pSpriteFrame = CSpriteFrame::Create(_pDevice, _pContext, szDirPath,_infIle, _Textures);
		if (nullptr == pSpriteFrame)
			return E_FAIL;
		_uint iFrameRun = 0;
		_infIle.read(reinterpret_cast<char*>(&iFrameRun), sizeof(_uint));
		SpriteFrames.push_back({ pSpriteFrame,iFrameRun });
	}

	return S_OK;
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
	if (iCurrentFrame >= iFrameCount)
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

_float CAnimation2D::Get_Progress()
{
	_float fProgerss = 0;
	if (iFrameCount > 0)
	{
		fProgerss = (_float)(Get_AccumulativeSubFrameCount(iCurrentFrame) + iCurrentSubFrame) / (_float)Get_AccumulativeSubFrameCount(iFrameCount - 1);
	}
	return fProgerss;
}

_uint CAnimation2D::Get_AccumulativeSubFrameCount(_uint _iFrameIndex)
{
	_uint iAccumulativeSubFrames = 0;
	for (_uint i = 0; i < _iFrameIndex; i++)
	{
		iAccumulativeSubFrames += SpriteFrames[i].second;
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
	for (auto& pSpriteFrame : SpriteFrames)
	{
		Safe_Release(pSpriteFrame.first);
	}
	__super::Free();
}
