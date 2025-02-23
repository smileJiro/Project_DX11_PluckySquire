#include "stdafx.h"
#include "Shadow2D.h"
#include "2DModel.h"
#include "Controller_Model.h"
#include "Shader.h"


CShadow2D::CShadow2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CShadow2D::CShadow2D(const CShadow2D& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CShadow2D::Initialize(void* _pArg)
{
	SHADOW2D_DESC* pDesc = static_cast<SHADOW2D_DESC*>(_pArg);
	m_pTargetObject = pDesc->pTargetObject;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	return S_OK;
}


HRESULT CShadow2D::Render()
{
	return __super::Render();
}

HRESULT CShadow2D::Bind_ShaderResources_WVP()
{
	_matrix matLocal = *static_cast<C2DModel*>(m_pControllerModel->Get_Model(COORDINATE_2D))->Get_CurrentSpriteTransform();
	_matrix matRatioScalling = XMMatrixScaling((_float)RATIO_BOOK2D_X, (_float)RATIO_BOOK2D_Y, 1.f);
	matLocal *= matRatioScalling;

	_matrix matWorld = matLocal * XMLoadFloat4x4(&m_WorldMatrices[COORDINATE_2D]);

	_float4x4 matWorld4x4;
	XMStoreFloat4x4(&matWorld4x4, matWorld);
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &matWorld4x4)))
		return E_FAIL;
	return S_OK;
}

CShadow2D* CShadow2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CShadow2D* pInstance = new CShadow2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Shadow2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShadow2D::Clone(void* _pArg)
{
	CShadow2D* pInstance = new CShadow2D(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Shadow2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadow2D::Free()
{
	__super::Free();
}
