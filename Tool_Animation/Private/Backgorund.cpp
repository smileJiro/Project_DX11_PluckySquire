#include "stdafx.h"
#include "Backgorund.h"
#include "GameInstance.h"
#include "2DModel.h"

CBackgorund::CBackgorund(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{

	XMStoreFloat4x4(&m_matView, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matProj , XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY,0.f, 1.f));
}


CBackgorund::CBackgorund(const CBackgorund& _Prototype)
	: CModelObject(_Prototype)
	, m_matView(_Prototype.m_matView)
	, m_matProj(_Prototype.m_matProj)
{
}

void CBackgorund::Late_Update(_float _fTimeDelta)
{
	if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
		Register_RenderGroup(RG_3D, PR3D_GEOMETRY);
	else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
		Register_RenderGroup(RG_3D, PR3D_UI);
	CPartObject::Late_Update(_fTimeDelta);
}

//HRESULT CBackgorund::Render()
//{
//	if (FAILED(Bind_ShaderResources_WVP()))
//		return E_FAIL;
//	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
//	CShader* pShader = m_pShaderComs[eCoord];
//	_uint iShaderPass = m_iShaderPasses[eCoord];
//	if (COORDINATE_3D == eCoord)
//		pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));
//	m_pControllerModel->Render(pShader, iShaderPass);
//	return S_OK;
//}

//HRESULT CBackgorund::Bind_ShaderResources_WVP()
//{
//	_matrix matLocal = *static_cast<C2DModel*>(m_pControllerModel->Get_Model(COORDINATE_2D))->Get_CurrentSpriteTransform();
//
//	_matrix matWorld = matLocal * XMLoadFloat4x4(&m_WorldMatrices[COORDINATE_2D]);
//
//	_float4x4 matWorld4x4;
//	XMStoreFloat4x4(&matWorld4x4, matWorld);
//	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &matWorld4x4)))
//		return E_FAIL;
//	//if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
//	//	return E_FAIL;
//
//	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_matView)))
//		return E_FAIL;
//	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_matProj)))
//		return E_FAIL;
//	return S_OK;
//}

CBackgorund* CBackgorund::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBackgorund* pInstance = new CBackgorund(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackgorund");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBackgorund* CBackgorund::Clone(void* _pArg)
{
	CBackgorund* pInstance = new CBackgorund(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackgorund");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackgorund::Free()
{
	__super::Free();
}
