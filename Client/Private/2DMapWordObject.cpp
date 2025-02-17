#include "stdafx.h"
#include "2DMapWordObject.h"
#include "GameInstance.h"


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



    if(WordObjectJson.contains("Image_Tag_List") && WordObjectJson["Image_Tag_List"].is_array())
    {}
    if(WordObjectJson.contains(""))
    {}    
    if(WordObjectJson.contains(""))
    {}    
    if(WordObjectJson.contains(""))
    {}


    return S_OK;
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
  
    return __super::Render();
}

HRESULT C2DMapWordObject::Render_Shadow()
{
    return S_OK;
}

HRESULT C2DMapWordObject::Action_Execute(_uint _iContainerIndex, _uint iWordIndex)
{
    auto pAction = Find_Action(_iContainerIndex, iWordIndex);

    if (nullptr != pAction)
    {
        switch (pAction->eAction)
        {
        case IMAGE_CHANGE: 
        {
            _uint iImageIndex = any_cast<_uint>(pAction->anyParam);
            m_iModelIndex = iImageIndex;
        }
            break;
        case WORD_OBJECT_ACTIVE:
        {
            _bool isActive = any_cast<_bool>(pAction->anyParam);
            Set_Active(isActive);
        }
        default:
            break;
        }
    
    }
    return S_OK;
}

const C2DMapWordObject::WORD_ACTION* C2DMapWordObject::Find_Action(_uint _iContainerIndex, _uint _iWordIndex)
{
    auto iter = find_if(m_Actions.begin(), m_Actions.end(), [&_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction)->_bool {
            return tAction.iContainerIndex == _iContainerIndex && tAction.iWordType == _iWordIndex;
        });

    if (m_Actions.end() != iter)
    {
        return &(*iter);
    }

    return nullptr;
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
