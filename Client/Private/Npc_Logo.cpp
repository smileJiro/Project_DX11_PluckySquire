#include "stdafx.h"
#include "NPC_Logo.h"
#include "2DModel.h"
#include "GameInstance.h"

#include "UI_Manager.h"

CNPC_Logo::CNPC_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CUI(_pDevice, _pContext)
{
}

CNPC_Logo::CNPC_Logo(const CNPC_Logo& _Prototype)
	:CUI(_Prototype)
{
}



HRESULT CNPC_Logo::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Logo::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);
	//pDesc->eStartCoord = COORDINATE_2D;
	//pDesc->isCoordChangeEnable = true;
	//pDesc->iCurLevelID = LEVEL_LOGO;
	//pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	//pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;
	//
	//pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	//pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;

	m_iCurLevelID = LEVEL_LOGO;
	m_isRender = true;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimationLoop(24, true);
	m_pModelCom->Set_Animation(24);

	return S_OK;
}

void CNPC_Logo::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNPC_Logo::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
	m_pModelCom->Play_Animation(_fTimeDelta, false);
}

void CNPC_Logo::Late_Update(_float _fTimeDelta)
{
	Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	//__super::Late_Update(_fTimeDelta);

}

HRESULT CNPC_Logo::Render()
{	
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	m_pModelCom->Render(m_pShaderCom, (_uint)PASS_VTXPOSTEX::SPRITE2D);
	int a = 0;
	//Register_RenderGroup()

	return S_OK;
}




void CNPC_Logo::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

}


HRESULT CNPC_Logo::Ready_Components()
{	
	// 셰이더를 만든다.
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// 모델쪽 생성하는 관련 부분 // 모델 게임레벨
	CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_LOGO, TEXT("Prototype_Component_NPC_Pip_2DAnimation"), nullptr));

	if (nullptr == pComponent)
		return E_FAIL;

	m_pModelCom = static_cast<C2DModel*>(pComponent);

	return S_OK;
}



CNPC_Logo* CNPC_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_Logo* pInstance = new CNPC_Logo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_Logo::Clone(void* _pArg)
{
	CNPC_Logo* pInstance = new CNPC_Logo(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_Logo Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_Logo::Free()
{
	__super::Free();
}

HRESULT CNPC_Logo::Cleanup_DeadReferences()
{
	return S_OK;
}
