#include "CubeMap.h"
#include "GameInstance.h"

CCubeMap::CCubeMap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CCubeMap::CCubeMap(const CCubeMap& _Prototype)
	:CGameObject(_Prototype)
{
}

HRESULT CCubeMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCubeMap::Initialize(void* _pArg)
{
	CUBEMAP_DESC* pDesc = static_cast<CUBEMAP_DESC*>(_pArg);
	// Save Desc
	m_iRenderGroupID = pDesc->iRenderGroupID;
	m_iPriorityID = pDesc->iPriorityID;

	// Add Desc
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.vInitialPosition = { 0.0f, 0.0f, 0.0f };
	pDesc->tTransform3DDesc.vInitialScaling = { 50.0f, 50.0f, 50.0f };

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if(FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	// Create ConstantBuffer
	if (FAILED(m_pGameInstance->CreateConstBuffer(m_tPixelConstData, D3D11_USAGE_DYNAMIC, &m_pPixeConstBuffer)))
		return E_FAIL;

	// debug일땐 그리고 아닐땐 안그려도되겠지.
	m_isRender = true;

	return S_OK;
}

void CCubeMap::Priority_Update(_float _fTimeDelta)
{
}

void CCubeMap::Update(_float _fTimeDelta)
{
}

void CCubeMap::Late_Update(_float _fTimeDelta)
{
	// Camera의 위치를 따라다니는 처리가 필요할 수 있겠지.

	//m_pControllerTransform->Set_Scale(_float3(700.f, 700.f, 700.f));
	// RenderGroup 추가.
	//m_pGameInstance->Add_RenderObject_New(m_iRenderGroupID, 30, this);
}

HRESULT CCubeMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	switch (m_tPixelConstData.iDrawTextureIndex)
	{
	case Engine::CCubeMap::TEX_ENV:
		m_pCubeTexturesCom->Bind_ShaderResource(m_pShaderCom, "g_EnvTexture", TEX_ENV);
		break;
	case Engine::CCubeMap::TEX_SPEC:
		m_pCubeTexturesCom->Bind_ShaderResource(m_pShaderCom, "g_SpecularTexture", TEX_SPEC);
		break;
	case Engine::CCubeMap::TEX_IRR:
		m_pCubeTexturesCom->Bind_ShaderResource(m_pShaderCom, "g_IrradianceTexture", TEX_IRR);
		break;
	default:
		break;
	}


	m_pShaderCom->Bind_ConstBuffer("PixelConstData", m_pPixeConstBuffer);

	m_pShaderCom->Begin((_uint)PASS_VTXCUBE::HDRI_ENV); 

	m_pVIBufferCom->Bind_BufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCubeMap::Ready_Components(CUBEMAP_DESC* _pDesc)
{
	// 1. cube map texture를 보관할 각각의 Texture Component

	/* Com_Texture_Cube */
	_int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
	if (FAILED(Add_Component(iStaticLevelID, _pDesc->strCubeMapPrototypeTag,
		TEXT("Com_Texture_Cube"), reinterpret_cast<CComponent**>(&m_pCubeTexturesCom))))
		return E_FAIL;

	/* Com_Texture_BRDF */
	if (FAILED(Add_Component(iStaticLevelID, _pDesc->strBRDFPrototypeTag,
		TEXT("Com_Texture_BRDF"), reinterpret_cast<CComponent**>(&m_pBRDFTextureCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_Shader_VtxCube"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(iStaticLevelID, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCubeMap::Bind_ShaderResources()
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CCubeMap::Bind_IBLTexture(CShader* _pShaderCom, const _char* _pBRDFConstName, const _char* _pSpecularConstName, const _char* _pIrradianceConstName)
{
	if(FAILED(m_pBRDFTextureCom->Bind_ShaderResource(_pShaderCom, _pBRDFConstName, 0)))
		return E_FAIL;

	if (FAILED(m_pCubeTexturesCom->Bind_ShaderResource(_pShaderCom, _pSpecularConstName, TEX_TYPE::TEX_SPEC)))
		return E_FAIL;

	if (FAILED(m_pCubeTexturesCom->Bind_ShaderResource(_pShaderCom, _pIrradianceConstName, TEX_TYPE::TEX_IRR)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCubeMap::Change_CubeMap(const _wstring& _strCubeMapPrototypeTag)
{
	Remove_Component(TEXT("Com_Texture_Cube"));
	Safe_Release(m_pCubeTexturesCom);



	assert(!m_pCubeTexturesCom); // null이 아니면 assert
	/* Com_Texture_Cube */
	_int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
	if (FAILED(Add_Component(iStaticLevelID, _strCubeMapPrototypeTag,
		TEXT("Com_Texture_Cube"), reinterpret_cast<CComponent**>(&m_pCubeTexturesCom))))
		return E_FAIL;

	return S_OK;
}

CCubeMap* CCubeMap::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCubeMap* pInstance = new CCubeMap(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCubeMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCubeMap::Clone(void* _pArg)
{
	CCubeMap* pInstance = new CCubeMap(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCubeMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCubeMap::Free()
{
	Safe_Release(m_pCubeTexturesCom);
	Safe_Release(m_pBRDFTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pPixeConstBuffer);

	__super::Free();
}

HRESULT CCubeMap::Cleanup_DeadReferences()
{
	return S_OK;
}
