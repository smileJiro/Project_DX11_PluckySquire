#include "stdafx.h"
#include "Monster_Body.h"
#include "GameInstance.h"
#include "FSM.h"
#include "DetectionField.h"

CMonster_Body::CMonster_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CMonster_Body::CMonster_Body(const CMonster_Body& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CMonster_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_Body::Initialize(void* _pArg)
{
	if (FAILED(Ready_Desc(&_pArg)))
		return E_FAIL;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CMonster_Body::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta); /* Part Object Priority_Update */
}

void CMonster_Body::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta); /* Part Object Update */
}

void CMonster_Body::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta); /* Part Object Late_Update */
}

HRESULT CMonster_Body::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonster_Body::Cleanup_DeadReferences()
{
	if (FAILED(__super::Cleanup_DeadReferences()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Body::Active_OnEnable()
{
	// 1. PxActor 활성화 (활성화 시점에는 먼저 켜고)
	CActorObject::Active_OnEnable();



	// 2. 몬스터 할거 하고
//	m_pTarget = m_pGameInstance->Get_GameObject_Ptr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0);
//	if (nullptr == m_pTarget)
//	{
//#ifdef _DEBUG
//		cout << "MONSTERINIT : NO PLAYER" << endl;
//#endif // _DEBUG
//		return;
//	}
//
//	Safe_AddRef(m_pTarget);


}

void CMonster_Body::Active_OnDisable()
{
	// 1. 몬스터 할거 하고


	// 2. PxActor 비활성화 
	CActorObject::Active_OnDisable();
}

HRESULT CMonster_Body::Ready_Desc(void** _pArg)
{
	/* Part Body */
	CModelObject::MODELOBJECT_DESC* BodyDesc = static_cast<MODELOBJECT_DESC*>(*_pArg);

	if (false == BodyDesc->isCoordChangeEnable)
	{
		if (COORDINATE_2D == BodyDesc->eStartCoord)
		{
			BodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
			BodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
			BodyDesc->iRenderGroupID_2D = RG_3D;
			BodyDesc->iPriorityID_2D = PR3D_BOOK2D;
		}
		else if (COORDINATE_3D == BodyDesc->eStartCoord)
		{
			BodyDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
			BodyDesc->iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;
			BodyDesc->iRenderGroupID_3D = RG_3D;
			BodyDesc->iPriorityID_3D = PR3D_NONBLEND;
		}
	}
	else
	{
		BodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
		BodyDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");

		BodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
		BodyDesc->iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;

		/* 태웅 : 렌더러 관련 추가 */
		BodyDesc->iRenderGroupID_2D = RG_3D;
		BodyDesc->iPriorityID_2D = PR3D_BOOK2D;
		BodyDesc->iRenderGroupID_3D = RG_3D;
		BodyDesc->iPriorityID_3D = PR3D_NONBLEND;
	}

	

	return S_OK;
}

CMonster_Body* CMonster_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CMonster_Body* pInstance = new CMonster_Body(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_Body::Clone(void* _pArg)
{
	CMonster_Body* pInstance = new CMonster_Body(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster_Body");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMonster_Body::Free()
{
	__super::Free();
}
