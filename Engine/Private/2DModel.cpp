#include "2DModel.h"
#include "GameInstance.h"
#include "Engine_Defines.h"

C2DModel::C2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModel(_pDevice, _pContext)
{
}

C2DModel::C2DModel(const C2DModel& _Prototype)
	:CModel(_Prototype)
	, m_pNonAnimTexture(_Prototype.m_pNonAnimTexture)
	, m_vNonAnimSpriteStartUV(_Prototype.m_vNonAnimSpriteStartUV)
	, m_vNonAnimSpriteEndUV(_Prototype.m_vNonAnimSpriteEndUV)
	, m_iCurAnimIdx(_Prototype.m_iCurAnimIdx)
	, m_pVIBufferCom(_Prototype.m_pVIBufferCom)
{
	for (auto& pAnim : _Prototype.m_Animation2Ds)
	{
		m_Animation2Ds.push_back(pAnim->Clone());
	}
}



HRESULT C2DModel::Initialize_Prototype(const _char* pModelFilePath)
{
	json jAnimation2D;
	std::ifstream input_file(pModelFilePath);
	if (!input_file.is_open())
		return E_FAIL;
	input_file >> jAnimation2D;
	input_file.close();
	m_eAnimType = jAnimation2D["AnimType"];
	std::filesystem::path path;
	path = pModelFilePath;
	path = path.remove_filename();
	if (ANIM == m_eAnimType)
	{
		json& jAnimations = jAnimation2D["Animations"];
		for (auto& j : jAnimations)
		{
			string jAnimFileName = path.string();
			jAnimFileName += "Frames/";
			jAnimFileName += j;
			json jAnimation;
			std::ifstream input_file(jAnimFileName.c_str());
			if (!input_file.is_open())
				return E_FAIL;
			input_file >> jAnimation;
			input_file.close();
			m_Animation2Ds.push_back(CAnimation2D::Create(m_pDevice,m_pContext,path.string(), jAnimation.front()));
		}
	}
	else
	{
		string strSourceTexture = pModelFilePath;
		strSourceTexture += "Textures/";
		strSourceTexture += jAnimation2D["TextureName"] + ".png";
		wstring wstrSourceTexture = CGameInstance::GetInstance()->StringToWString(strSourceTexture);
		m_pNonAnimTexture = CTexture::Create(m_pDevice, m_pContext, wstrSourceTexture.c_str());
		m_vNonAnimSpriteStartUV = { jAnimation2D["SpriteStartUV"][0],jAnimation2D["SpriteStartUV"][1] };
		m_vNonAnimSpriteEndUV = { jAnimation2D["SpriteEndUV"][0],jAnimation2D["SpriteEndUV"][1] };
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
		if (m_pNonAnimTexture)
			if (FAILED(m_pNonAnimTexture->Bind_ShaderResource(_pShader, "g_Texture")))
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
	__super::Free();
}
