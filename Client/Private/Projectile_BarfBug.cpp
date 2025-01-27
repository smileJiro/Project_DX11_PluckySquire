#include "stdafx.h"
#include "Projectile_BarfBug.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "GameInstance.h"

CProjectile_BarfBug::CProjectile_BarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CProjectile_BarfBug::CProjectile_BarfBug(const CProjectile_BarfBug& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CProjectile_BarfBug::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_BarfBug::Initialize(void* _pArg)
{
    PROJECTILE_BARFBUG_DESC* pDesc = static_cast<PROJECTILE_BARFBUG_DESC*>(_pArg);

    m_fLifeTime = pDesc->fLifeTime;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

	return S_OK;
}

void CProjectile_BarfBug::Priority_Update(_float _fTimeDelta)
{
    m_fAccTime += _fTimeDelta;

    __super::Priority_Update(_fTimeDelta);
}

void CProjectile_BarfBug::Update(_float _fTimeDelta)
{
	if (false == Is_Dead() && m_fLifeTime <= m_fAccTime)
    {
        m_fAccTime = 0.f;
        Event_DeleteObject(this);
    }
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CProjectile_BarfBug::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);
}

HRESULT CProjectile_BarfBug::Render()
{
    __super::Render();
    return S_OK;
}

void CProjectile_BarfBug::Active_OnEnable()
{
}

void CProjectile_BarfBug::Active_OnDisable()
{
    m_pControllerTransform->Set_WorldMatrix(XMMatrixIdentity());
    m_fAccTime = 0.f;
}

HRESULT CProjectile_BarfBug::Ready_Components()
{
    return S_OK;
}

HRESULT CProjectile_BarfBug::Ready_PartObjects()
{
    CModelObject::MODELOBJECT_DESC BodyDesc{};

    BodyDesc.eStartCoord = m_pControllerTransform->Get_CurCoord();
    BodyDesc.iCurLevelID = m_iCurLevelID;
    BodyDesc.isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();

    BodyDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    BodyDesc.strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    BodyDesc.strModelPrototypeTag_2D = TEXT("barferbug_projectile");
    BodyDesc.strModelPrototypeTag_3D = TEXT("S_FX_CMN_Sphere_01");
    BodyDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
    BodyDesc.iModelPrototypeLevelID_3D = m_iCurLevelID;
    BodyDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE_ANIM;
    BodyDesc.iShaderPass_3D = (_uint)PASS_VTXMESH::TEST_PROJECTILE;

    BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
    BodyDesc.pParentMatrices[COORDINATE_3D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_3D);

    BodyDesc.tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform3DDesc.vInitialScaling = _float3(0.5f, 0.5f, 0.5f);
    BodyDesc.tTransform3DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform3DDesc.fSpeedPerSec = 10.f;

    BodyDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    BodyDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
    BodyDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(90.f);
    BodyDesc.tTransform2DDesc.fSpeedPerSec = 10.f;

    m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &BodyDesc));
    if (nullptr == m_PartObjects[PART_BODY])
        return E_FAIL;

    return S_OK;
}

CProjectile_BarfBug* CProjectile_BarfBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CProjectile_BarfBug* pInstance = new CProjectile_BarfBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_BarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_BarfBug::Clone(void* _pArg)
{
    CProjectile_BarfBug* pInstance = new CProjectile_BarfBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_BarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_BarfBug::Free()
{

    __super::Free();
}
