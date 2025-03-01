#include "stdafx.h"
#include "Boss_HomingBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

CBoss_HomingBall::CBoss_HomingBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_HomingBall::CBoss_HomingBall(const CBoss_HomingBall& _Prototype)
	: CProjectile_Monster(_Prototype)
{
}

HRESULT CBoss_HomingBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_HomingBall::Initialize(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);

    m_fOriginSpeed = pDesc->tTransform3DDesc.fSpeedPerSec;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

	return S_OK;
}

void CBoss_HomingBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_HomingBall::Update(_float _fTimeDelta)
{

    _vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    //테스트용, 원 게임에서는 구체인데 일단 불릿 모델 써봄
	if (2.f >= m_fAccTime)
    {
        /*m_pControllerTransform->Set_AutoRotationYDirection(vDir);
        m_pControllerTransform->Update_AutoRotation(_fTimeDelta);*/
    }
    else
    {
		m_pControllerTransform->Set_SpeedPerSec(m_fOriginSpeed * 2.f);
    }
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CBoss_HomingBall::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_HomingBall::Render()
{
    __super::Render();
    return S_OK;
}

HRESULT CBoss_HomingBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "HOMINGBALL_Cleanup : NO PLAYER" << endl;
#endif // _DEBUG
        return S_OK;
    }

    if (true == m_pTarget->Is_Dead())
    {
        Safe_Release(m_pTarget);
        m_pTarget = nullptr;
    }

    return S_OK;
}

void CBoss_HomingBall::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CBoss_HomingBall::Active_OnDisable()
{
    m_pControllerTransform->Set_SpeedPerSec(m_fOriginSpeed);

    __super::Active_OnDisable();
}

HRESULT CBoss_HomingBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_HomingBall::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("HomingBall");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::TEST_PROJECTILE;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(10.f, 10.f, 10.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBoss_HomingBall* CBoss_HomingBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_HomingBall* pInstance = new CBoss_HomingBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_HomingBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_HomingBall::Clone(void* _pArg)
{
    CBoss_HomingBall* pInstance = new CBoss_HomingBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_HomingBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_HomingBall::Free()
{
    Safe_Release(m_pTarget);

    __super::Free();
}
