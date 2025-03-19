#include "stdafx.h"
#include "CyberCursor.h"
#include "GameInstance.h"
CCyberCursor::CCyberCursor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CCyberCursor::CCyberCursor(const CCyberCursor& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CCyberCursor::Initialize(void* _pArg)
{
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.0f, 1.0f));


	CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);
	m_iCurLevelID = pBodyDesc->iCurLevelID;
	pBodyDesc->eStartCoord = COORDINATE_2D;
	pBodyDesc->isCoordChangeEnable = false;

	pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(100.f, 100.f, 1.f);

	pBodyDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
	pBodyDesc->strModelPrototypeTag_2D = TEXT("Prototype_Model2D_CyberCursor");

	pBodyDesc->eActorType = ACTOR_TYPE::LAST;
	pBodyDesc->pActorDesc = nullptr;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	Set_CursorPos(_vector{1.f,10.f,1.f,1.f });
	return S_OK;
}

void CCyberCursor::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
	Update_CursorPos();
}

void CCyberCursor::Late_Update(_float _fTimeDelta)
{
	Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	__super::Late_Update(_fTimeDelta);
}

HRESULT CCyberCursor::Render()
{

	return __super::Render();
}

void CCyberCursor::Set_CursorPos(_fvector _v3DPos)
{

	m_vWorldPos = _v3DPos;
}

HRESULT CCyberCursor::Bind_ShaderResources_WVP()
{

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_2D])))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	return S_OK;
}

void CCyberCursor::Update_CursorPos()
{
	m_vWorldPos.m128_f32[3] = 1.f;
	_matrix matView = XMLoadFloat4x4(&m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	_matrix matProj = XMLoadFloat4x4(&m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));

	//WorldPos = XMVector4Transform(WorldPos, matView);
	//WorldPos = XMVector4Transform(WorldPos, matProj);
	//WorldPos /= WorldPos.m128_f32[3];
	m_vWorldPos = XMVector3TransformCoord(m_vWorldPos, matView * matProj);
	m_vWorldPos.m128_f32[0] = (m_vWorldPos.m128_f32[0]) * g_iWinSizeX * 0.5;
	m_vWorldPos.m128_f32[1] = (m_vWorldPos.m128_f32[1]) * g_iWinSizeY * 0.5;
	m_vWorldPos.m128_f32[2] = 0.1f;
	m_vWorldPos.m128_f32[3] = 1.f;

	m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_State(CTransform::STATE_POSITION, m_vWorldPos);

}


CCyberCursor* CCyberCursor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCyberCursor* pInstance = new CCyberCursor(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCyberCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCyberCursor::Clone(void* _pArg)
{
	CCyberCursor* pInstance = new CCyberCursor(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCyberCursor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCyberCursor::Free()
{
	__super::Free();
}
