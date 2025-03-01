#include "stdafx.h"
#include "Boss_EnergyBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

CBoss_EnergyBall::CBoss_EnergyBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_EnergyBall::CBoss_EnergyBall(const CBoss_EnergyBall& _Prototype)
    : CProjectile_Monster(_Prototype)
{
}

HRESULT CBoss_EnergyBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_EnergyBall::Initialize(void* _pArg)
{
    PROJECTILE_MONSTER_DESC* pDesc = static_cast<PROJECTILE_MONSTER_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

	return S_OK;
}

void CBoss_EnergyBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_EnergyBall::Update(_float _fTimeDelta)
{

    /*_vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);*/
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CBoss_EnergyBall::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_EnergyBall::Render()
{
    __super::Render();
    return S_OK;
}

HRESULT CBoss_EnergyBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "ENERGYBALL_Cleanup : NO PLAYER" << endl;
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

void CBoss_EnergyBall::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CBoss_EnergyBall::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CBoss_EnergyBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_EnergyBall::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::TEST_PROJECTILE;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(2.f, 2.f, 2.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBoss_EnergyBall* CBoss_EnergyBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_EnergyBall* pInstance = new CBoss_EnergyBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_EnergyBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_EnergyBall::Clone(void* _pArg)
{
    CBoss_EnergyBall* pInstance = new CBoss_EnergyBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_EnergyBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_EnergyBall::Free()
{
    Safe_Release(m_pTarget);

    __super::Free();
}
