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

    if (_pArg != nullptr)
    {
        MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);
        m_matWorld = pDesc->tTransform3DDesc.matWorld;

        if (pDesc->is2DImport)
        {
            auto tInfo = CSection_Manager::GetInstance()->Get_2DModel_Info(pDesc->i2DModelIndex);
            // 콜라이더, 소팅, 액티브, 오버라이드 등은 나중에... 일단 띄워놓기만 합니다.
            pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
            pDesc->strModelPrototypeTag_2D = StringToWstring(tInfo.strModelName);
            //pDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;

        }
    }
    // MODELOBJ
    {
        MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

        // Save 
        m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
        m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;
        m_strModelPrototypeTag[COORDINATE_2D] = pDesc->strModelPrototypeTag_2D;
        m_strModelPrototypeTag[COORDINATE_3D] = pDesc->strModelPrototypeTag_3D;
        m_fFrustumCullingRange = pDesc->fFrustumCullingRange;

        m_iRenderGroupID_2D = pDesc->iRenderGroupID_2D;
        m_iPriorityID_2D = pDesc->iPriorityID_2D;
        m_iRenderGroupID_3D = pDesc->iRenderGroupID_3D;
        m_iPriorityID_3D = pDesc->iPriorityID_3D;


        if (FAILED(CModelObject::Ready_Components(pDesc)))
            return E_FAIL;

        XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

        _float2 fRTSize = m_pGameInstance->Get_RT_Size(L"Target_Book_2D");

        XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)fRTSize.x, (_float)fRTSize.y, 0.0f, 1.0f));
    }
    
    CActor::ACTOR_DESC ActorDesc;
    SHAPE_COOKING_DESC ShapeCookingDesc = {};
    SHAPE_DATA ShapeData;

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

    if (FAILED(CPartObject::Initialize(_pArg)))
        return E_FAIL;


    return S_OK;
}

void CMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}
void CMapObject::Update(_float _fTimeDelta)
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        if (nullptr != m_pParentMatrices[COORDINATE_2D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_2D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D));
        break;
    case Engine::COORDINATE_3D:
        if (nullptr != m_pParentMatrices[COORDINATE_3D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D));
        break;
    default:
        break;
    }

    CGameObject::Update_Component(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{
    //switch (m_pControllerTransform->Get_CurCoord())
    //{
    //case Engine::COORDINATE_2D:
    //    if (nullptr != m_pParentMatrices[COORDINATE_2D])
    //        XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_2D]));
    //    else
    //        XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D));
    //    break;
    //case Engine::COORDINATE_3D:
    //    if (nullptr != m_pParentMatrices[COORDINATE_3D])
    //        XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
    //    else
    //        XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D));
    //    break;
    //default:
    //    break;
    //}

    CGameObject::Late_Update_Component(_fTimeDelta);

    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
    {
        if (true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 0.0f))
        {
            Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_NONBLEND);


        }
        else
        {

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
