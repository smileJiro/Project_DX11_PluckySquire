#include "stdafx.h"
#include "CyberPlayerBullet.h"
#include "Character.h"
#include "3DModel.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Effect_Manager.h"
#include "FresnelModelObject.h"
#include "Trail_Manager.h"
#include "Boss_TennisBall.h"
#include "ButterGrump_Shield.h"

CCyberPlayerBullet::CCyberPlayerBullet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CCyberPlayerBullet::CCyberPlayerBullet(const CCyberPlayerBullet& _Prototype)
	: CContainerObject(_Prototype)
	, m_pFresnelBuffer(_Prototype.m_pFresnelBuffer)
	, m_pColorBuffer(_Prototype.m_pColorBuffer)
{
	Safe_AddRef(m_pFresnelBuffer);
	Safe_AddRef(m_pColorBuffer);
}

HRESULT CCyberPlayerBullet::Initialize_Prototype()
{
	FRESNEL_INFO tBulletFresnelInfo = {};
	tBulletFresnelInfo.fBaseReflect = 0.04f;
	tBulletFresnelInfo.fExp = 0.19f;
	tBulletFresnelInfo.vColor = { 0.690f, 1.0f, 1.0f, 1.f };
	tBulletFresnelInfo.fStrength = 1.f; // 안씀.
	m_pGameInstance->CreateConstBuffer(tBulletFresnelInfo, D3D11_USAGE_DEFAULT, &m_pFresnelBuffer);


	COLORS_INFO tColorsInfo = {};
	tColorsInfo.vDiffuseColor = _float4(0.2f, 0.8f, 1.f, 1.f);
	tColorsInfo.vBloomColor = _float4(0.27f, 0.83f, 1.f, 1.f);
	m_pGameInstance->CreateConstBuffer(tColorsInfo, D3D11_USAGE_DEFAULT, &m_pColorBuffer);

	return S_OK;
}

HRESULT CCyberPlayerBullet::Initialize(void* _pArg)
{
	CYBERPLAYER_PROJECTILE_DESC* pDesc = static_cast<CYBERPLAYER_PROJECTILE_DESC*>(_pArg);
	pDesc->iNumPartObjects = BULLET_END;
	pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER_PROJECTILE;
	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.vInitialScaling = _float3(1, 1, 1);
	pDesc->tTransform3DDesc.fSpeedPerSec = m_fSpeed;

    pDesc->eActorType = ACTOR_TYPE::KINEMATIC;
    CActor::ACTOR_DESC ActorDesc;

    /* Actor의 주인 오브젝트 포인터 */
    ActorDesc.pOwner = this;

    /* Actor의 회전축을 고정하는 파라미터 */
    ActorDesc.FreezeRotation_XYZ[0] = true;
    ActorDesc.FreezeRotation_XYZ[1] = true;
    ActorDesc.FreezeRotation_XYZ[2] = true;

    /* Actor의 이동축을 고정하는 파라미터 (이걸 고정하면 중력도 영향을 받지 않음. 아예 해당 축으로의 이동을 제한하는)*/
    ActorDesc.FreezePosition_XYZ[0] = false;
    ActorDesc.FreezePosition_XYZ[1] = false;
    ActorDesc.FreezePosition_XYZ[2] = false;

    /* 사용하려는 Shape의 형태를 정의 */
    ActorDesc.ShapeDatas.resize(1);

	SHAPE_SPHERE_DESC tBodyShapeDesc = {};
     tBodyShapeDesc.fRadius = 0.25f;
	SHAPE_DATA tBodyShapeData = {};
    tBodyShapeData.pShapeDesc = &tBodyShapeDesc;       
    tBodyShapeData.eShapeType = SHAPE_TYPE::SPHERE;   
    tBodyShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;  
    tBodyShapeData.iShapeUse = (_uint)SHAPE_USE::SHAPE_BODY;
    tBodyShapeData.isTrigger = true;                  
    tBodyShapeData.FilterData.MyGroup = OBJECT_GROUP::PLAYER_PROJECTILE;
	tBodyShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::BOSS | OBJECT_GROUP::BOSS_PROJECTILE;
	XMStoreFloat4x4(&tBodyShapeData.LocalOffsetMatrix, XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.0f, 0.f, 0.0f));

    /* 최종으로 결정 된 ShapeData를 PushBack */
    ActorDesc.ShapeDatas[tBodyShapeData.iShapeUse] = tBodyShapeData;

    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::PLAYER_PROJECTILE;
    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::BOSS | OBJECT_GROUP::BOSS_PROJECTILE ;

    /* Actor Component Finished */
    pDesc->pActorDesc = &ActorDesc;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	CFresnelModelObject::FRESNEL_MODEL_DESC tModelDesc{};
	tModelDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
	tModelDesc.strModelPrototypeTag_3D = TEXT("CyberPlayerBullet");
	tModelDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	tModelDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::FRESNEL;
	tModelDesc.iRenderGroupID_3D = RG_3D;
	tModelDesc.iPriorityID_3D = PR3D_PARTICLE;
	tModelDesc.eStartCoord = COORDINATE_3D;
	tModelDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);
	// 이 버퍼를 이용해서 Render 합니다.
	tModelDesc.pColorBuffer = m_pColorBuffer;
	tModelDesc.pFresnelBuffer = m_pFresnelBuffer;

	m_PartObjects[PART_BODY] = m_pModel= static_cast<CFresnelModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_FresnelModelObject"), &tModelDesc));
	if (nullptr == m_PartObjects[PART_BODY])
	{
		MSG_BOX("CPlayer VISOR Creation Failed");
		return E_FAIL;
	}
	Safe_AddRef(m_pModel);
	m_PartObjects[PART_BODY]->Set_Position({ 0.f,0.f,0.f });
	m_PartObjects[PART_BODY]->Get_ControllerTransform()->Rotation(XMConvertToRadians(90.f), _vector{ 0.f,1.f,0.f,0.f });
	Set_PartActive(PART_BODY, true);

	m_vLookDIr = XMVector3Normalize( m_pControllerTransform->Get_WorldMatrix().r[2]);
	static_cast<C3DModel*>(m_pModel->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_UseAlbedoMap(0, false, true);
	static_cast<C3DModel*>(m_pModel->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(0.f, 1.f, 1.f, 1.f), true);

	// Trail Particle? Effect
	CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};
	EffectDesc.eStartCoord = COORDINATE_3D;
	EffectDesc.isCoordChangeEnable = false;
	EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
	EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
	EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
	EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
	EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";

	m_PartObjects[BULLET_TRAIL] =  m_pParticleTrailEffect = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("BulletTrail.json"), &EffectDesc));
	if (nullptr != m_pParticleTrailEffect)
	{
		m_pParticleTrailEffect->Set_SpawnMatrix(m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D));
		m_pParticleTrailEffect->Active_All();
	}
	Safe_AddRef(m_pParticleTrailEffect);

	return S_OK;
}

void CCyberPlayerBullet::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CCyberPlayerBullet::Update(_float _fTimeDelta)
{
	m_fLifeTimeAcc += _fTimeDelta;
	if (m_fLifeTimeAcc >= m_fLifeTime)
	{
		Event_DeleteObject(this);
		m_fLifeTimeAcc = 0.f;
	}
	m_pControllerTransform->Go_Straight(_fTimeDelta);
	__super::Update(_fTimeDelta);
}

void CCyberPlayerBullet::Active_OnEnable()
{
	if (nullptr != m_pParticleTrailEffect)
		m_pParticleTrailEffect->Active_Effect(true);

	__super::Active_OnEnable();
}

void CCyberPlayerBullet::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{	//여긴 MONSTER 아니면 MONSTER_PROJECTILE만 충돌됨.
	if (OBJECT_GROUP::BOSS & _Other.pActorUserData->iObjectGroup
		|| OBJECT_GROUP::BOSS_PROJECTILE & _Other.pActorUserData->iObjectGroup)
	{
		Event_Hit(this, static_cast<CCharacter*>(_Other.pActorUserData->pOwner), m_iDamg, _vector{ 0.f,0.f,0.f });
		Event_DeleteObject(this);


		if (nullptr != dynamic_cast<CBoss_TennisBall*>(_Other.pActorUserData->pOwner))
		{
			CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("BulletHitTennis"), true, Get_FinalPosition());
			m_pGameInstance->Start_SFX_Distance_Delay(_wstring(L"A_sfx_jot_laser_hits_tennisball-") + to_wstring(rand() % 6), m_pControllerTransform->Get_State(CTransform::STATE_POSITION), 0.1f, g_SFXVolume, 0.f, 13.f);
		}
		else if (nullptr != dynamic_cast<CButterGrump_Shield*>(_Other.pActorUserData->pOwner))
		{
			CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("BulletHitShield"), true, Get_FinalPosition());
			m_pGameInstance->Start_SFX_Distance_Delay(_wstring(L"A_sfx_impact_nodamage-") + to_wstring(rand() % 6), m_pControllerTransform->Get_State(CTransform::STATE_POSITION), 0.1f, g_SFXVolume, 0.f, 13.f);
		}
		else
		{
			CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("CyberBulletHit"), true, Get_FinalPosition());
		}
		if (nullptr != m_pParticleTrailEffect)
			m_pParticleTrailEffect->Inactive_All();
	}
}



CCyberPlayerBullet* CCyberPlayerBullet::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCyberPlayerBullet* pInstance = new CCyberPlayerBullet(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create CyberPlayerBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCyberPlayerBullet::Clone(void* _pArg)
{
	CGameObject* pInstance = new CCyberPlayerBullet(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Clone CyberPlayerBullet");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCyberPlayerBullet::Free()
{
	Safe_Release(m_pFresnelBuffer);
	Safe_Release(m_pColorBuffer);

	Safe_Release(m_pModel);
	Safe_Release(m_pParticleTrailEffect);
	__super::Free();
}
