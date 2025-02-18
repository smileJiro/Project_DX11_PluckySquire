#include "stdafx.h"
#include "2DMapWordObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"


C2DMapWordObject::C2DMapWordObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C2DMapObject(_pDevice, _pContext)
{
}

C2DMapWordObject::C2DMapWordObject(const C2DMapWordObject& _Prototype)
    :C2DMapObject(_Prototype)
{
}

HRESULT C2DMapWordObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DMapWordObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;
    WORD_OBJECT_DESC* pDesc = static_cast<WORD_OBJECT_DESC*>(_pArg);

    json& WordObjectJson = pDesc->WordObjectJson;

    // 풀 텍스트
    _string strText = WordObjectJson["Word_Object_Tag"];
    m_strWordObjectTag = StringToWstring(strText);

    _float2 fPos = { WordObjectJson["Position"][0].get<_float>(),
        WordObjectJson["Position"][1].get<_float>() };

    _float2 fScale = { WordObjectJson["Scale"][0].get<_float>(),
        WordObjectJson["Scale"][1].get<_float>() };

    pDesc->Build_2D_Transform(fPos, fScale);

    if(WordObjectJson.contains("Image_Tag_List") 
        && WordObjectJson["Image_Tag_List"].is_array())
    {
        _uint iModelCount = (_uint)WordObjectJson["Image_Tag_List"].size();
        m_ModelNames.reserve(iModelCount);
        for (auto& ImageTagJson : WordObjectJson["Image_Tag_List"])
        {
            if (ImageTagJson.contains("ImagePath"))
            {
                _string strText = ImageTagJson["ImagePath"];
                m_ModelNames.push_back(StringToWstring(strText));
            }
        }

        if (!m_ModelNames.empty())
        {
            pDesc->Build_2D_Model(SECTION_MGR->Get_SectionLeveID(),
                m_ModelNames[0],
                TEXT("Prototype_Component_Shader_VtxPosTex")
                );
        }
    }


    if (WordObjectJson.contains("Word_Action_List")
        && WordObjectJson["Word_Action_List"].is_array())
    {
        for (auto& WordActionJson : WordObjectJson["Word_Action_List"])
        {
            WORD_ACTION tAction = {};
            if (WordActionJson.contains("Controller_Index"))
                tAction.iControllerIndex = WordActionJson["Controller_Index"];
            if (WordActionJson.contains("Container_Index"))
                tAction.iContainerIndex = WordActionJson["Container_Index"];

            if (WordActionJson.contains("Word_Type"))
                tAction.iWordType = WordActionJson["Word_Type"];

            if (WordActionJson.contains("Action_Type"))
                tAction.eAction = WordActionJson["Action_Type"];
            
            if (WordActionJson.contains("Param"))
            {
                if(WordActionJson["Param"].is_boolean())
                    tAction.anyParam = (_bool)WordActionJson["Param"];
                if(WordActionJson["Param"].is_number())
                    tAction.anyParam = (_uint)WordActionJson["Param"];
                if(WordActionJson["Param"].is_number_float())
                    tAction.anyParam = (_float)WordActionJson["Param"];
            }
            m_Actions.push_back(tAction);
        }
    }





    HRESULT hr =__super::Initialize(pDesc);



    if (WordObjectJson.contains("Collider_Info"))
    {
        if (WordObjectJson["Collider_Info"].contains("Collider_Extent"))
        {
            _float2 fExtent = { WordObjectJson["Collider_Info"]["Collider_Extent"][0].get<_float>(),
            WordObjectJson["Collider_Info"]["Collider_Extent"][1].get<_float>() };



            /* Test 2D Collider */
            m_p2DColliderComs.resize(1);
            CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
            AABBDesc.pOwner = this;
            AABBDesc.vExtents = fExtent;
            AABBDesc.vScale = {1.f,1.f};
            AABBDesc.vOffsetPosition = {0.f,0.f};
            AABBDesc.isBlock = true;
            AABBDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;
            CCollider_AABB* pCollider = nullptr;
            if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
                TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
                return E_FAIL;

        }
    
    }



}

void C2DMapWordObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DMapWordObject::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);

}

void C2DMapWordObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapWordObject::Render()
{
#ifdef _DEBUG
    if (!m_p2DColliderComs.empty())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
    return __super::Render();
}

HRESULT C2DMapWordObject::Render_Shadow()
{
    return S_OK;
}

HRESULT C2DMapWordObject::Action_Execute(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
    for_each(m_Actions.begin(), m_Actions.end(), [this, &_iControllerIndex, &_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction){
            if (tAction.iControllerIndex == _iControllerIndex
                && tAction.iContainerIndex == _iContainerIndex
                && tAction.iWordType == _iWordIndex)
            {
                switch (tAction.eAction)
                {
                case IMAGE_CHANGE:
                {
                    _uint iImageIndex = any_cast<_uint>(tAction.anyParam);

                    if (m_iModelIndex != iImageIndex)
                        m_pControllerModel->Change_Model(COORDINATE_2D, SECTION_MGR->Get_SectionLeveID(), m_ModelNames[iImageIndex]);
                    m_iModelIndex = iImageIndex;
                }
                break;
                case WORD_OBJECT_ACTIVE:
                {
                    _bool isActive = any_cast<_bool>(tAction.anyParam);
                    m_IsWordActive = isActive;
                    Set_Render(m_IsWordActive);
                }
                break;
                case ANIMATION_CHANGE:
                {
                    _uint iAnimIndex = any_cast<_uint>(tAction.anyParam);
                    Set_AnimationLoop(COORDINATE_2D, iAnimIndex, true);
                    Set_Animation(iAnimIndex);
                }
                break;
                case POSITION_CHANGE_X:
                {
                    _float fPostionX = any_cast<_float>(tAction.anyParam);
                    _vector vOriginalPosition = Get_FinalPosition();
                    Set_Position(XMVectorSetX(vOriginalPosition, fPostionX));
                }
                break;
                case POSITION_CHANGE_Y:
                {
                    _float fPostionY = any_cast<_float>(tAction.anyParam);
                    _vector vOriginalPosition = Get_FinalPosition();
                    Set_Position(XMVectorSetY(vOriginalPosition, fPostionY));
                }
                break;                
                case COLLIDER_ACTIVE:
                {
                    _bool isActive = any_cast<_bool>(tAction.anyParam);
                    if (!m_p2DColliderComs.empty() && nullptr != m_p2DColliderComs[0])
                        m_p2DColliderComs[0]->Set_Active(isActive);
                }
                break;
                default:
                    break;
                }
            
            }
        });
    return S_OK;
}

const C2DMapWordObject::WORD_ACTION* C2DMapWordObject::Find_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
    auto iter = find_if(m_Actions.begin(), m_Actions.end(), [&_iControllerIndex ,&_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction)->_bool {
            return tAction.iControllerIndex == _iControllerIndex
                && tAction.iContainerIndex == _iContainerIndex
                && tAction.iWordType == _iWordIndex;
                
        });

    if (m_Actions.end() != iter)
    {
        return &(*iter);
    }

    return nullptr;
}

void C2DMapWordObject::Active_OnEnable()
{
    if (m_IsWordActive != true)
        Set_Active(m_IsWordActive);
}

void C2DMapWordObject::Active_OnDisable()
{
    if (m_IsWordActive != false)
        Set_Active(m_IsWordActive);
}


C2DMapWordObject* C2DMapWordObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DMapWordObject* pInstance = new C2DMapWordObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DMapWordObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DMapWordObject::Clone(void* _pArg)
{
    C2DMapWordObject* pInstance = new C2DMapWordObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DMapWordObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DMapWordObject::Free()
{
    __super::Free();
}
