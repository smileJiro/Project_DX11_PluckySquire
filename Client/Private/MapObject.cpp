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
        if (pDesc->is2DImport)
        {
            auto tInfo = CSection_Manager::GetInstance()->Get_2DModel_Info(pDesc->i2DModelIndex);
            // 콜라이더, 소팅, 액티브, 오버라이드 등은 나중에... 일단 띄워놓기만 합니다.
            pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
            pDesc->strModelPrototypeTag_2D = StringToWstring(tInfo.strModelName);
            //pDesc->iModelPrototypeLevelID_2D = m_iCurLevelID;
        }
    }


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    return S_OK;
}

void CMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}
void CMapObject::Update(_float _fTimeDelta)
{
    CModelObject::Update(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{
    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
    {
        //if(true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_Position(), 5.0f))
        //      m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);

        //if (true == m_pGameInstance->isIn_Frustum_InWorldSpace(Get_Position(), 5.0f))
            m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_NONBLEND, this);
       
    }
        
    //else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
    //    m_pGameInstance->Add_RenderObject_New(RG_3D, PR3D_BOOK2D, this);
        //m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);


    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
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
