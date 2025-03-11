#include "stdafx.h"
#include "Effect_Beam.h"
#include "GameInstance.h"
#include "Client_Defines.h"

CEffect_Beam::CEffect_Beam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CPartObject(_pDevice, _pContext)
{
}

CEffect_Beam::CEffect_Beam(const CEffect_Beam& _Prototype)
	: CPartObject(_Prototype)
{
}

HRESULT CEffect_Beam::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Beam::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	EFFECTBEAM_DESC* pDesc = static_cast<EFFECTBEAM_DESC*>(_pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;


	m_vColor = pDesc->vColor;
	m_fWidth = pDesc->fWidth;
	m_isConverge = pDesc->isConverge;
	m_fConvergeSpeed = pDesc->fConvergeSpeed;
	m_isRenderPoint = pDesc->isRenderPoint;
	m_fPointSize = pDesc->fPointSize;
	m_vPointColor = pDesc->vPointColor;
	m_pStartPointBoneMatrix = pDesc->pStartPointBoneMatrix;

	return S_OK;
}

void CEffect_Beam::Update(_float _fTimeDelta)
{

#ifdef _DEBUG
	ImGui::Begin("Beam");
	ImGui::DragFloat4("Color", (_float*)&m_vColor, 0.01f);
	ImGui::DragFloat("Width", &m_fWidth, 0.01f);
	ImGui::DragFloat4("PointColor", (_float*)&m_vPointColor, 0.01f);
	ImGui::DragFloat("PointSize", &m_fPointSize, 0.01f);

	ImGui::End();
#endif 
	if (false == m_isActive)
		return;

	if (m_isConverge)
		m_pBufferCom->Converge_Points(m_fConvergeSpeed * _fTimeDelta);

	__super::Update(_fTimeDelta);
}

void CEffect_Beam::Late_Update(_float _fTimeDelta)
{
	if (false == m_isActive)
		return;

	__super::Late_Update(_fTimeDelta);

	if (m_pStartPointBoneMatrix)
	{
		_matrix matWorld = XMLoadFloat4x4(m_pStartPointBoneMatrix) * XMLoadFloat4x4(&m_WorldMatrices[COORDINATE_3D]);

		m_pBufferCom->Set_StartPosition(XMVectorSetW(matWorld.r[3], 1.f), true);
	}
	
	m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_PARTICLE, this);
}

HRESULT CEffect_Beam::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pBufferCom->Bind_BufferDesc()))
		return E_FAIL;
	if (FAILED(m_pBufferCom->Render()))
		return E_FAIL;

	if (m_isRenderPoint && nullptr != m_pPointTextureCom)
	{
		m_pShaderCom->Bind_RawValue("g_fPointSize", &m_fPointSize, sizeof(_float));
		m_pShaderCom->Bind_RawValue("g_vColor", &m_vPointColor, sizeof(_float4));
		if (FAILED(m_pPointTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
			return E_FAIL;
	
		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;
		if (FAILED(m_pBufferCom->Bind_PointBufferDesc()))
			return E_FAIL;
		if (FAILED(m_pBufferCom->Render_Points()))
			return E_FAIL;
	}


	return S_OK;
}

void CEffect_Beam::Set_StartPosition(_fvector _vStartPosition, _bool _isUpdateAll)
{
	m_pBufferCom->Set_StartPosition(_vStartPosition, _isUpdateAll);
}

void CEffect_Beam::Set_EndPosition(_fvector _vEndPosition, _bool _isUpdateAll)
{
	m_pBufferCom->Set_EndPosition(_vEndPosition, _isUpdateAll);
}



HRESULT CEffect_Beam::Bind_ShaderResources()
{
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWidth", &m_fWidth, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	_float4 vLook;
	XMStoreFloat4(&vLook, m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW).r[2]);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLook", &vLook, sizeof(_float4))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

HRESULT CEffect_Beam::Ready_Components(const EFFECTBEAM_DESC* _pDesc)
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Beam"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CVIBuffer_Beam::VIBUFFERBEAM_DESC Desc = {};
	Desc.vRandomMin = _pDesc->vRandomMin;
	Desc.vRandomMax = _pDesc->vRandomMax;

	if (FAILED(Add_Component(LEVEL_STATIC, _pDesc->szBufferTag,
		TEXT("Com_Buffer"), reinterpret_cast<CComponent**>(&m_pBufferCom), &Desc)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_STATIC, _pDesc->szTextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	if (_pDesc->isRenderPoint)
	{
		if (FAILED(Add_Component(LEVEL_STATIC, _pDesc->szPointTextureTag,
			TEXT("Com_PointTexture"), reinterpret_cast<CComponent**>(&m_pPointTextureCom))))
			return E_FAIL;
	}




	return S_OK;
}

CEffect_Beam* CEffect_Beam::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect_Beam* pInstance = new CEffect_Beam(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Beam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Beam::Clone(void* _pArg)
{
	CEffect_Beam* pInstance = new CEffect_Beam(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Beam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Beam::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);

	__super::Free();
}

HRESULT CEffect_Beam::Cleanup_DeadReferences()
{
	return S_OK;
}
