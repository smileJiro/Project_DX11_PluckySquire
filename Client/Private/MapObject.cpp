#include "stdafx.h"
#include "MapObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"


CMapObject::CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMapObject::CMapObject(const CMapObject& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMapObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMapObject::Initialize(void* _pArg)
{

    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

    if (nullptr == pDesc)
        return E_FAIL;

    #pragma region 기본 필수 Desc 채우기
        // 2D Import Object의 경우, Section_Manager의 2DModel 정보를 가져와서 채워줘야 한다.
        if (pDesc->is2DImport)
        {
            m_isSorting = pDesc->isSorting;
        }
        m_matWorld = pDesc->tTransform3DDesc.matWorld;
        m_is3DCulling = pDesc->is3DCulling;
    #pragma endregion

    //CModelObject::Initialize()의 Component 생성 구현부는 필요하나, 이후 super 클래스 Init 로직을 타기 전에 처리해야 하는 정보가 있기 때문에, 부득이 뺴서 구현.
    //                                                                    (생성된 모델을 확인하여야 쿠킹 셰잎 생성이 가능하다.)
    #pragma region CModelObject::Initialize(pArg); 재구현부
        // Save 
        m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
        m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;
        m_strModelPrototypeTag[COORDINATE_2D] = pDesc->strModelPrototypeTag_2D;
        m_strModelPrototypeTag[COORDINATE_3D] = pDesc->strModelPrototypeTag_3D;
        m_fFrustumCullingRange = pDesc->fFrustumCullingRange;

        m_iRenderGroupID_3D = pDesc->iRenderGroupID_3D;
        m_iPriorityID_3D = pDesc->iPriorityID_3D;


        if (FAILED(CModelObject::Ready_Components(pDesc)))
            return E_FAIL;

    #pragma endregion

  
    // 구조체는 if문 내부에 넣어서 포인터로 던지면, 사라지므로. 밖에 선언
    CActor::ACTOR_DESC ActorDesc;
    SHAPE_COOKING_DESC ShapeCookingDesc = {};
    SHAPE_DATA ShapeData;

    // 모델에 쿠킹 셰잎 정보가 있으면, 액터 정보를 ModelDesc에 포함시킨다.
    #pragma region CActorObject::Initialize(pArg) 준비작업 
        if (nullptr != m_pControllerModel)
        {
            CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);

            if (nullptr != pModel)
            {
                if (static_cast<C3DModel*>(pModel)->Has_CookingCollider())
                {

                    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

                    pDesc->eActorType = ACTOR_TYPE::STATIC;

                    ActorDesc.pOwner = this;

                    ActorDesc.FreezeRotation_XYZ[0] = true;
                    ActorDesc.FreezeRotation_XYZ[1] = true;
                    ActorDesc.FreezeRotation_XYZ[2] = true;

                    ActorDesc.FreezePosition_XYZ[0] = false;
                    ActorDesc.FreezePosition_XYZ[1] = false;
                    ActorDesc.FreezePosition_XYZ[2] = false;

                    ShapeCookingDesc.isLoad = true;
                    ShapeCookingDesc.isSave = false;
                    ShapeData.eShapeType = SHAPE_TYPE::COOKING;
                    ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
                    ShapeData.pShapeDesc = &ShapeCookingDesc;
                    ShapeData.isTrigger = false;
                    _float3 fScale =
                        _float3(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_matWorld.m[0]))),
                            XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_matWorld.m[1]))),
                            XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_matWorld.m[2]))));
                    _matrix matScale = XMMatrixScaling(fScale.x, fScale.y, fScale.z);
                    XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, matScale);
                    ActorDesc.ShapeDatas.push_back(ShapeData);
                    pDesc->pActorDesc = &ActorDesc;

                    ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::MAPOBJECT;
                    ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::PLAYER;
                
                }
            }
        }
    #pragma endregion

    // 이후 CModelObject를 제외한 super 클래스 initialize를 다시 태운다.
    if (FAILED(CPartObject::Initialize(_pArg)))
        return E_FAIL;

    if (Get_CurCoord() == COORDINATE_2D)
    {
        auto pModel = Get_ModelController()->Get_Model(COORDINATE_2D);
        if (pModel->Get_AnimType() == CModel::ANIM)
        {
            Set_AnimationLoop(COORDINATE_2D, 0, true);
            Set_Animation(0);
        }
    }

    return S_OK;
}

void CMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}
void CMapObject::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{
    CGameObject::Late_Update_Component(_fTimeDelta);

    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
    {
        if (!m_is3DCulling || true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 5.0f))
        {
            Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_NONBLEND);

            //if(nullptr != m_pActorCom)
            //    m_pActorCom->Set_ShapeRayCastFlag(true);
        }
        else
        {
            //if (nullptr != m_pActorCom)
            //    m_pActorCom->Set_ShapeRayCastFlag(false);
        }
    }
            

}

HRESULT CMapObject::Render()
{
    switch (m_eColorShaderMode)
    {
        case Engine::C3DModel::COLOR_DEFAULT:
            if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vDefaultDiffuseColor", &m_fDefaultDiffuseColor, sizeof(_float4))))
                return E_FAIL;
            m_iShaderPasses[COORDINATE_3D] = 3;
            break;
        case Engine::C3DModel::MIX_DIFFUSE:
            if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vDefaultDiffuseColor", &m_fDefaultDiffuseColor, sizeof(_float4))))
                return E_FAIL;
                m_iShaderPasses[COORDINATE_3D] = 4;
            break;
        default:
                m_iShaderPasses[COORDINATE_3D] = 0;
            break;
    }




    CModelObject::Render();
    return S_OK;
}

HRESULT CMapObject::Render_Shadow()
{
    return S_OK;
}

#ifdef _DEBUG

//
//HRESULT CMapObject::Load_Override_Material(HANDLE _hFile)
//{
//
//    DWORD	dwByte(0);
//    _uint iOverrideCount = 0;
//
//    ReadFile(_hFile, &iOverrideCount, sizeof(_uint), &dwByte, nullptr);
//    if (0 < iOverrideCount)
//    {
//        for (_uint i = 0; i < iOverrideCount; i++)
//        {
//            OVERRIDE_MATERIAL_INFO tInfo = {};
//            ReadFile(_hFile, &tInfo.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
//            ReadFile(_hFile, &tInfo.iTexTypeIndex, sizeof(_uint), &dwByte, nullptr);
//            ReadFile(_hFile, &tInfo.iTexIndex, sizeof(_uint), &dwByte, nullptr);
//
//            Change_TextureIdx(tInfo.iTexIndex, tInfo.iTexTypeIndex, tInfo.iMaterialIndex);
//        }
//    }
//    return S_OK;
//}

//
//HRESULT CMapObject::Load_Override_Color(HANDLE _hFile)
//{
//    DWORD	dwByte(0);
//    ReadFile(_hFile, &m_eColorShaderMode, sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);
//    switch (m_eColorShaderMode)
//    {
//    case Engine::C3DModel::COLOR_DEFAULT:
//    case Engine::C3DModel::MIX_DIFFUSE:
//        ReadFile(_hFile, &m_fDefaultDiffuseColor, sizeof(_float4), &dwByte, nullptr);
//        break;
//    default:
//        break;
//    }
//    return S_OK;
//}



#endif // _DEBUG



CMapObject* CMapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMapObject* pInstance = new CMapObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        //MSG_BOX("Failed to Created : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapObject::Clone(void* _pArg)
{
    CMapObject* pInstance = new CMapObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        //MSG_BOX("Failed to Cloned : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapObject::Free()
{
    __super::Free();
}
