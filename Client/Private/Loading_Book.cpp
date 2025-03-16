#include "stdafx.h"
#include "Loading_Book.h"
#include "2DModel.h"
#include "GameInstance.h"

#include "UI_Manager.h"

CLoading_Book::CLoading_Book(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CUI(_pDevice, _pContext)
{
}

CLoading_Book::CLoading_Book(const CLoading_Book& _Prototype)
	:CUI(_Prototype)
{
}



HRESULT CLoading_Book::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoading_Book::Initialize(void* _pArg)
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

void CLoading_Book::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CLoading_Book::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
	m_pModelCom->Play_Animation(_fTimeDelta, false);
}

void CLoading_Book::Late_Update(_float _fTimeDelta)
{
	Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	//__super::Late_Update(_fTimeDelta);

}

HRESULT CLoading_Book::Render()
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




void CLoading_Book::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

}


HRESULT CLoading_Book::Ready_Components()
{	
	// 셰이더를 만든다.
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	// 모델쪽 생성하는 관련 부분 // 모델 게임레벨
	CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, LEVEL_STATIC, TEXT("Loading_Book"), nullptr));

	if (nullptr == pComponent)
		return E_FAIL;

	m_pModelCom = static_cast<C2DModel*>(pComponent);

	return S_OK;
}



CLoading_Book* CLoading_Book::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLoading_Book* pInstance = new CLoading_Book(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLoading_Book::Clone(void* _pArg)
{
	CLoading_Book* pInstance = new CLoading_Book(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLoading_Book Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLoading_Book::Free()
{
	__super::Free();
}

HRESULT CLoading_Book::Cleanup_DeadReferences()
{
	return S_OK;
}
