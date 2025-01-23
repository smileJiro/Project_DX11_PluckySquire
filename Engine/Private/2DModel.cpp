#include "2DModel.h"
#include "GameInstance.h"
#include "Engine_Defines.h"

C2DModel::C2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModel(_pDevice, _pContext)
{
}

C2DModel::C2DModel(const C2DModel& _Prototype)
	:CModel(_Prototype)
	, m_NonAnimTextures(_Prototype.m_NonAnimTextures)
	, m_vNonAnimSpriteStartUV(_Prototype.m_vNonAnimSpriteStartUV)
	, m_vNonAnimSpriteEndUV(_Prototype.m_vNonAnimSpriteEndUV)
	, m_iCurAnimIdx(_Prototype.m_iCurAnimIdx)
	, m_pVIBufferCom(_Prototype.m_pVIBufferCom)

{
	for (auto& pAnim : _Prototype.m_Animation2Ds)
	{
		m_Animation2Ds.push_back(pAnim->Clone());
	}
	for (auto& pTex : m_NonAnimTextures)
	{
		Safe_AddRef(pTex);
	}

}

HRESULT C2DModel::Initialize_Prototype(const _char* _szModel2DFilePath)
{
	std::ifstream inFile(_szModel2DFilePath, std::ios::binary);
	if (!inFile) {
		string str = "파일을 열 수 없습니다.";
		str += _szModel2DFilePath;
		MessageBoxA(NULL, str.c_str(), "에러", MB_OK);
		return E_FAIL;
	}
	_char		szDrive[MAX_PATH] = "";
	_char		szDirectory[MAX_PATH] = "";
	_splitpath_s(_szModel2DFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szDrive, szDirectory);

	//AllTextures
	_uint iCount = 0;//TextureCount
	inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	for (_uint i = 0; i < iCount; i++)
	{
		//TextureName Length
		_uint iLength = 0;
		inFile.read(reinterpret_cast<char*>(&iLength), sizeof(_uint));
		_char szTextureName[MAX_PATH] = "";
		inFile.read(szTextureName, iLength);
		szTextureName[iLength] = '\0';
		if (m_Textures.find(szTextureName) != m_Textures.end())
			continue;
		ID3D11ShaderResourceView* pSRV = { nullptr };
		std::filesystem::path path = szDrive;
		path += szTextureName;
		path += ".png";
		CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, path.c_str());
		if (nullptr == pTexture)
			return E_FAIL;
		pTexture->Add_Texture(pSRV, path.filename().replace_extension().wstring());
		m_Textures.insert({ szTextureName,pTexture });
	}

	//Animation2Ds
	inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	m_Animation2Ds.reserve(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		CAnimation2D* pAnimation = CAnimation2D::Create(m_pDevice, m_pContext, szDrive,inFile, m_Textures);
		if (nullptr == pAnimation)
			return E_FAIL;
		m_Animation2Ds.push_back(pAnimation);
	}
	m_eAnimType = m_Animation2Ds.size() > 0 ? ANIM : NONANIM;
	//NonANimTextures
	inFile.read(reinterpret_cast<char*>(&iCount), sizeof(_uint));
	m_NonAnimTextures.reserve(iCount);
	for (_uint i = 0; i < iCount; i++)
	{
		_uint iLength = 0;
		inFile.read(reinterpret_cast<char*>(&iLength), sizeof(_uint));
		_char szTextureName[MAX_PATH];
		inFile.read(szTextureName, iLength);
		szTextureName[iLength] = '\0';
		auto& pairTexture = m_Textures.find(szTextureName);
		if (pairTexture == m_Textures.end())
			return E_FAIL;
		m_NonAnimTextures.push_back(pairTexture->second);
	}
	//NonAnimSpriteStartUV
	inFile.read(reinterpret_cast<char*>(&m_vNonAnimSpriteStartUV), sizeof(_float2));
	//NonAnimSpriteEndUV
	inFile.read(reinterpret_cast<char*>(&m_vNonAnimSpriteEndUV), sizeof(_float2));
	return S_OK;
}

HRESULT C2DModel::Initialize(void* _pDesc)
{
	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;

    /* Com_VIBuffer */
	m_pVIBufferCom = static_cast<CVIBuffer_Rect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_pGameInstance->Get_StaticLevelID(), TEXT("Prototype_Component_VIBuffer_Rect"), nullptr));
    if (nullptr == m_pVIBufferCom)
        return E_FAIL;


	return S_OK;
}

void C2DModel::Set_AnimationLoop(_uint _iIdx, _bool _bIsLoop)
{
	m_Animation2Ds[_iIdx]->Set_Loop(_bIsLoop);
}

void C2DModel::Set_Animation(_uint _iIdx)
{
	m_iCurAnimIdx = _iIdx;
}

void C2DModel::Switch_Animation(_uint _iIdx)
{
	m_iCurAnimIdx = _iIdx;
	m_Animation2Ds[m_iCurAnimIdx]->Reset_CurrentTrackPosition();
}

void C2DModel::To_NextAnimation()
{
	Switch_Animation((m_iCurAnimIdx + 1) % m_Animation2Ds.size());
}

const _matrix* C2DModel::Get_CurrentSpriteTransform()
{
	return m_Animation2Ds[m_iCurAnimIdx]->Get_CurrentSpriteTransform();
}

_uint C2DModel::Get_AnimCount()
{
	return (_uint)m_Animation2Ds.size();
}


HRESULT C2DModel::Render(CShader* _pShader, _uint _iShaderPass)
{
	if (Is_AnimModel())
	{
		if (m_Animation2Ds[m_iCurAnimIdx])
			if (FAILED(m_Animation2Ds[m_iCurAnimIdx]->Bind_ShaderResource(_pShader)))
				return E_FAIL;
	}
	else
	{
		if (m_NonAnimTextures[0])
			if (FAILED(m_NonAnimTextures[0]->Bind_ShaderResource(_pShader, "g_Texture")))
				return E_FAIL;

		if (FAILED(_pShader->Bind_RawValue("g_vSpriteStartUV", &m_vNonAnimSpriteStartUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(_pShader->Bind_RawValue("g_vSpriteEndUV", &m_vNonAnimSpriteEndUV, sizeof(_float2))))
			return E_FAIL;
		_float fPixelsPerUnrealUnit =1;
		if (FAILED(_pShader->Bind_RawValue("g_fPixelsPerUnrealUnit", &fPixelsPerUnrealUnit, sizeof(_float))))
			return E_FAIL;
	}


	if (_pShader)
		if (FAILED(_pShader->Begin(_iShaderPass)))
			return E_FAIL;

	if (m_pVIBufferCom)
	{
		m_pVIBufferCom->Bind_BufferDesc();
		m_pVIBufferCom->Render();
	}
	else
		return E_FAIL;

	return S_OK;
}

_bool C2DModel::Play_Animation(_float _fTimeDelta)
{
	if (Is_AnimModel())
		return m_Animation2Ds[m_iCurAnimIdx]->Play_Animation(_fTimeDelta);
	return false;
}


C2DModel* C2DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath)
{
	C2DModel* pInstance = new C2DModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Created : 2DModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* C2DModel::Clone(void* _pArg)
{
	C2DModel* pInstance = new C2DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : 2DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DModel::Free()
{
	Safe_Release(m_pVIBufferCom);
	for (auto& pAnimation : m_Animation2Ds)
		Safe_Release(pAnimation);
	m_Animation2Ds.clear();
	for (auto& pTex : m_Textures)
		Safe_Release(pTex.second);
	m_Textures.clear();
	__super::Free();
}
