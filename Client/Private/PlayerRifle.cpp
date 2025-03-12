#include "stdafx.h"
#include "PlayerRifle.h"
#include "Pooling_Manager.h"
#include "Player.h"
#include "GameInstance.h"
#include "Effect_Manager.h"

CPlayerRifle::CPlayerRifle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CPlayerRifle::CPlayerRifle(const CPlayerRifle& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CPlayerRifle::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerRifle::Initialize(void* _pArg)
{
	m_pPoolMgr = CPooling_Manager::GetInstance();
	PLAYER_RIFLE_DESC* pDesc = static_cast<PLAYER_RIFLE_DESC*>(_pArg);
	m_pPlayer = pDesc->pPlayer;
	pDesc->eStartCoord = COORDINATE_3D;
	//pDesc->iCurLevelID = m_iCurLevelID;
	pDesc->isCoordChangeEnable =false;

	pDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
	pDesc->strModelPrototypeTag_3D = TEXT("Rifle_CyberJot");
	pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
	pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
	pDesc->iRenderGroupID_3D = RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	// Effect »ý¼º.
	CEffect_System::EFFECT_SYSTEM_DESC EffectDesc = {};
	EffectDesc.eStartCoord = COORDINATE_3D;
	EffectDesc.isCoordChangeEnable = false;
	EffectDesc.iSpriteShaderLevel = LEVEL_STATIC;
	EffectDesc.szSpriteShaderTags = L"Prototype_Component_Shader_VtxPointInstance";
	EffectDesc.iEffectShaderLevel = LEVEL_STATIC;
	EffectDesc.szEffectShaderTags = L"Prototype_Component_Shader_VtxMeshEffect";
	EffectDesc.szSpriteComputeShaderTag = L"Prototype_Component_Compute_Shader_SpriteInstance";

	m_pShotEffect = static_cast<CEffect_System*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, m_iCurLevelID, TEXT("CyberShot.json"), &EffectDesc));
	//if (nullptr == m_pShotEffect)
	//	return E_FAIL;
	if (nullptr != m_pShotEffect)
	{
		m_pShotEffect->Set_SpawnMatrix(&m_WorldMatrices[COORDINATE_3D]);
		m_pShotEffect->Set_Position(XMVectorSet(0.08f, 0.f, 0.5f, 1.f));
	}

	return S_OK;
}

void CPlayerRifle::Update(_float _fTimeDelta)
{
	m_fShootTimeAcc += _fTimeDelta;

	if (m_fShootTimeAcc > m_fShhotDelay)
	{
		m_fShootTimeAcc = m_fShhotDelay;
	}
	if (nullptr != m_pShotEffect)
	{
		m_fAccEffectDelay += _fTimeDelta;

		if (m_isEffect && 0.25f <= m_fAccEffectDelay)
		{
			m_fAccEffectDelay = 0.f;
			m_isEffect = false;

			m_pShotEffect->Stop_Spawn(0.15f);
		}

		m_pShotEffect->Update(_fTimeDelta);
	}
	
	__super::Update(_fTimeDelta);
}

void CPlayerRifle::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

	if (nullptr != m_pShotEffect)
		m_pShotEffect->Late_Update(_fTimeDelta);
}

void CPlayerRifle::Shoot()
{
	if (m_fShhotDelay <= m_fShootTimeAcc)
	{
		m_fShootTimeAcc = 0.f;
		_matrix matWorld = Get_FinalWorldMatrix();
		//POS
		_vector vFirePosition = matWorld.r[3] + m_vBarrelOffset;
		_float3 vPos; XMStoreFloat3(&vPos, vFirePosition);
		//ROT
		_vector vLook = m_pPlayer->Get_LookDirection();
	
		_vector vBase = _vector{ 0.f,0.f,1.f,0.f };
		_vector vAxis;
		//_vector vEqual = XMVectorNearEqual(vLook, vBase, XMVectorReplicate(0.001f));
		if (XMVector4EqualInt(vLook, vBase))
			vAxis = _vector{ 0.f,1.f,0.f,0.f };
		else
			vAxis= XMVector3Cross(vBase, vLook);

		_float fAngle = acosf(XMVectorGetX(XMVector3Dot(vBase, vLook)));
		_float4 vRot; XMStoreFloat4(&vRot, XMQuaternionRotationAxis(vAxis, fAngle));

		m_pPoolMgr->Create_Object(TEXT("Pooling_Projectile_CyberPlayerBullet"), COORDINATE_3D, &vPos, &vRot);

		// TEMP
		//CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("CyberShot"), false, vFirePosition);
		//CEffect_Manager::GetInstance()->Active_Effect(TEXT("CyberShot"), true, &m_WorldMatrices[COORDINATE_3D]);
		//CEffect_Manager::GetInstance()->Stop_Spawn(TEXT("CyberShot"), 0.5f);
		
		if (nullptr != m_pShotEffect)
		{
			m_isEffect = true;
			m_pShotEffect->Active_Effect(false);
		}

	}
}

CPlayerRifle* CPlayerRifle::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPlayerRifle* pInstance = new CPlayerRifle(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : PlayerRifle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerRifle::Clone(void* _pArg)
{
	CPlayerRifle* pInstance = new CPlayerRifle(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Clone : PlayerRifle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerRifle::Free()
{
	Safe_Release(m_pShotEffect);

	__super::Free();
}
