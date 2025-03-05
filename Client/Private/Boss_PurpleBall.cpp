#include "stdafx.h"
#include "Boss_PurpleBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

CBoss_PurpleBall::CBoss_PurpleBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CProjectile_Monster(_pDevice, _pContext)
{
}

CBoss_PurpleBall::CBoss_PurpleBall(const CBoss_PurpleBall& _Prototype)
    : CProjectile_Monster(_Prototype)
{
}

HRESULT CBoss_PurpleBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_PurpleBall::Initialize(void* _pArg)
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

void CBoss_PurpleBall::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_PurpleBall::Update(_float _fTimeDelta)
{

    /*_vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);*/
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CBoss_PurpleBall::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_PurpleBall::Render()
{
    __super::Render();
    return S_OK;
}

HRESULT CBoss_PurpleBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "PURPLEBALL_Cleanup : NO PLAYER" << endl;
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

void CBoss_PurpleBall::Active_OnEnable()
{
    __super::Active_OnEnable();
}

void CBoss_PurpleBall::Active_OnDisable()
{
    __super::Active_OnDisable();
}

HRESULT CBoss_PurpleBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_PurpleBall::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    BodyDesc.iRenderGroupID_3D = RG_3D;
    BodyDesc.iPriorityID_3D = PR3D_GEOMETRY;

    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(2.f, 2.f, 2.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    static_cast<C3DModel*>(static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_3D))->Set_MaterialConstBuffer_Albedo(0, _float4(0.f, 1.f, 1.f, 1.f), true);

    return S_OK;
}

CBoss_PurpleBall* CBoss_PurpleBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_PurpleBall* pInstance = new CBoss_PurpleBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_PurpleBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_PurpleBall::Clone(void* _pArg)
{
    CBoss_PurpleBall* pInstance = new CBoss_PurpleBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_PurpleBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_PurpleBall::Free()
{
    __super::Free();
}
