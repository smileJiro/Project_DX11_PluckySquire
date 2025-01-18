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
	, m_AnimTextures(_Prototype.m_AnimTextures)
{
	for (auto& pAnim : _Prototype.m_Animation2Ds)
	{
		m_Animation2Ds.push_back(pAnim->Clone());
	}
	for (auto& pTex : m_NonAnimTextures)
	{
		Safe_AddRef(pTex);
	}
	for (auto& pTex : m_AnimTextures)
	{
		Safe_AddRef(pTex.second);
	}
}



HRESULT C2DModel::Initialize_Prototype(const _char* _pModelDirectoryPath)
{
	//모든 json파일 순회하면서 읽음. 
	//Type이 PaperSprite인 경우와 PaperFlipBook인 경우로 컨테이너를 나눠서 저장.
	//	PaperFlipBook 컨테이너를 순회하면서	Animation2D를 생성.
	//    Animation2D를 생성할 때 PaperSprite 컨테이너에서 LookUp해서 CSpriteFrame생성.

	map<string,json> jPaperFlipBooks;
	map<string, json> jPaperSprites;


	std::filesystem::path path;
	path = _pModelDirectoryPath;
	json jFile;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.path().extension() == ".json") 
		{
			//cout << entry.path().string() << endl;
			std::ifstream input_file(entry.path());
			if (!input_file.is_open())
				return E_FAIL;
			input_file >> jFile;
			input_file.close();

			for (auto& jObj : jFile)
			{
				string strType = jObj["Type"];
				if (strType._Equal("PaperSprite"))
				{
					string strName = jObj["Name"];

					jPaperSprites.insert({ strName, jObj });
				}
				else if (strType._Equal("PaperFlipbook"))
				{
					string strName = jObj["Name"];

					jPaperFlipBooks.insert({ strName,jObj });
				}
				else
				{
					continue;
				}
			}

		}
		else if (entry.path().extension() == ".png") 
		{
			m_AnimTextures.insert({ entry.path().filename().replace_extension().string(), CTexture::Create(m_pDevice, m_pContext, entry.path().c_str()) });
		}
	}


	m_eAnimType = jPaperFlipBooks.size() > 0 ? ANIM : NONANIM;

	if (ANIM == m_eAnimType)
	{
		for (auto& j : jPaperFlipBooks)
		{
			string strName = j.second["Name"];

			m_Animation2Ds.push_back(CAnimation2D::Create(m_pDevice, m_pContext, j.second,jPaperSprites, m_AnimTextures));
		}
	}
	else
	{
		for (auto& pTex : m_AnimTextures)
		{
			m_NonAnimTextures.push_back(pTex.second);
		}
	}
	return S_OK;
}

HRESULT C2DModel::Initialize(void* _pDesc)
{
	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;

    /* Com_VIBuffer */
	m_pVIBufferCom = static_cast<CVIBuffer_Rect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, 1, TEXT("Prototype_Component_VIBuffer_Rect"), nullptr));
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
}

void C2DModel::To_NextAnimation()
{
	Switch_Animation((m_iCurAnimIdx + 1) % m_Animation2Ds.size());
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
		MSG_BOX("Failed to Created : C2DModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* C2DModel::Clone(void* _pArg)
{
	C2DModel* pInstance = new C2DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : C2DModel");
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
	for (auto& pTex : m_AnimTextures)
		Safe_Release(pTex.second);
	m_AnimTextures.clear();
	for (auto& pTex : m_NonAnimTextures)
		Safe_Release(pTex);
	m_NonAnimTextures.clear();
	__super::Free();
}
