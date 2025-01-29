#include "stdafx.h"
#include "Boss_YellowBall.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

CBoss_YellowBall::CBoss_YellowBall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CBoss_YellowBall::CBoss_YellowBall(const CBoss_YellowBall& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CBoss_YellowBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_YellowBall::Initialize(void* _pArg)
{
    BOSS_YELLOWBALL_DESC* pDesc = static_cast<BOSS_YELLOWBALL_DESC*>(_pArg);

    m_fLifeTime = pDesc->fLifeTime;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    

    //플레이어 위치 가져오기
    m_pTarget = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, TEXT("Layer_Player"), 0);
    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "YELLOWBALL_INIT : NO PLAYER" << endl;
#endif // _DEBUG
        return S_OK;
    }

    Safe_AddRef(m_pTarget);

	return S_OK;
}

void CBoss_YellowBall::Priority_Update(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;

    __super::Priority_Update(_fTimeDelta);
}

void CBoss_YellowBall::Update(_float _fTimeDelta)
{
	if (false == Is_Dead() && m_fLifeTime <= m_fAccTime)
    {
        m_fAccTime = 0.f;
        Event_DeleteObject(this);
    }

    /*_vector vDir = m_pTarget->Get_FinalPosition() - Get_FinalPosition();
    m_pControllerTransform->Go_Direction(vDir, _fTimeDelta);*/
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CBoss_YellowBall::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CBoss_YellowBall::Render()
{
    __super::Render();
    return S_OK;
}

HRESULT CBoss_YellowBall::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    if (nullptr == m_pTarget)
    {
#ifdef _DEBUG
        cout << "YELLOWBALL_Cleanup : NO PLAYER" << endl;
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

void CBoss_YellowBall::Active_OnEnable()
{
}

void CBoss_YellowBall::Active_OnDisable()
{
    //m_pControllerTransform->Set_State(CTransform_3D::STATE_POSITION, _float4(0.f, 0.f, 0.f, 1.f));
    _float4x4 matWorld;
    XMStoreFloat4x4(&matWorld, XMMatrixIdentity());
    m_pControllerTransform->Set_WorldMatrix(matWorld);
    m_fAccTime = 0.f;
}

HRESULT CBoss_YellowBall::Ready_Components()
{
    return S_OK;
}

HRESULT CBoss_YellowBall::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    //BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    //BodyDesc.strModelPrototypeTag_2D = TEXT("barfBug_Rig");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    //BodyDesc.iModelPrototypeLevelID_2D = LEVEL_GAMEPLAY;
    BodyDesc.iModelPrototypeLevelID_3D = LEVEL_GAMEPLAY;
    //BodyDesc.iShaderPass_2D = (_uint)PASS_VTXMESH::DEFAULT;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::TEST_PROJECTILE;

    //BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(2.f, 2.f, 2.f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    //BodyDesc->tTransform2DDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    //BodyDesc->tTransform2DDesc.vScaling = _float3(1.0f, 1.0f, 1.0f);
    //BodyDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    //BodyDesc->tTransform2DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CBoss_YellowBall* CBoss_YellowBall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_YellowBall* pInstance = new CBoss_YellowBall(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CBoss_YellowBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_YellowBall::Clone(void* _pArg)
{
    CBoss_YellowBall* pInstance = new CBoss_YellowBall(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoss_YellowBall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_YellowBall::Free()
{
	if (nullptr != m_pTarget)
        Safe_Release(m_pTarget);

    __super::Free();
}
