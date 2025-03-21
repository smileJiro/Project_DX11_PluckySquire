#include "stdafx.h"
#include "BackGroundObject.h"
#include "GameInstance.h"

CBackGroundObject::CBackGroundObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CBackGroundObject::CBackGroundObject(const CBackGroundObject& _Prototype)
	:CGameObject(_Prototype)
{
}

HRESULT CBackGroundObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackGroundObject::Initialize(void* _pArg)
{
	CBackGroundObject::BACKGROUNDOBJ_DESC* pDesc = static_cast<CBackGroundObject::BACKGROUNDOBJ_DESC*>(_pArg);


	// Add
	pDesc->eShaderPass = PASS_VTXMESH::BACKGROUND;
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->iObjectGroupID = OBJECT_GROUP::NONE;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CBackGroundObject::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CBackGroundObject::Update(_float _fTimeDelta)
{

	__super::Update(_fTimeDelta);
}

void CBackGroundObject::Late_Update(_float _fTimeDelta)
{
	Register_RenderGroup(RG_3D, PR3D_GEOMETRY);

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBackGroundObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin((_uint)m_eShaderPass);

	m_pVIBufferCom->Bind_BufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBackGroundObject::Chage_BackGroundTexture(const _wstring& _strTexturePrototypeTag)
{
	if(FAILED(Remove_Component(TEXT("Com_Texture"))))
		return E_FAIL;

	Safe_Release(m_pTextureCom);
	m_pTextureCom = nullptr;

	if (_strTexturePrototypeTag.empty())
	{
		/* Com_Texture */
		if (FAILED(Add_Component(m_iCurLevelID, _strTexturePrototypeTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			/* Level_Static */
			_int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
			if (FAILED(Add_Component(iStaticLevelID, _strTexturePrototypeTag,
				TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
				return E_FAIL;
		}

	}

	return S_OK;
}

HRESULT CBackGroundObject::Ready_Components(BACKGROUNDOBJ_DESC* _pDesc)
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Mesh"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (!_pDesc->strTexturePrototypeTag.empty())
	{
		/* Com_Texture */
		if (FAILED(Add_Component(m_iCurLevelID, _pDesc->strTexturePrototypeTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			/* Level_Static */
			_int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
			if (FAILED(Add_Component(iStaticLevelID, _pDesc->strTexturePrototypeTag,
				TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
				return E_FAIL;
		}

	}

	return S_OK;
}

HRESULT CBackGroundObject::Bind_ShaderResources()
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBackGroundColor", &m_vFilterColor, sizeof(_float3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightness", &m_fBrightness, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CBackGroundObject* CBackGroundObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBackGroundObject* pInstance = new CBackGroundObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CBackGroundObject Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackGroundObject::Clone(void* _pArg)
{
	CBackGroundObject* pInstance = new CBackGroundObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CBackGroundObject Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGroundObject::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}

HRESULT CBackGroundObject::Cleanup_DeadReferences()
{

	return S_OK;
}
