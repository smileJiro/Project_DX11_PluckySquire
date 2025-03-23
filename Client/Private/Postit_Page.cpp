#include "stdafx.h"
#include "Postit_Page.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CPostit_Page::CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CPostit_Page::CPostit_Page(const CPostit_Page& _Prototype)
    : m_eAnimAction(_Prototype.m_eAnimAction),
    CModelObject(_Prototype)
{
    std::copy(begin(_Prototype.m_Positions), end(_Prototype.m_Positions), m_Positions);
}


HRESULT CPostit_Page::Initialize_Prototype()
{
    // °Á ½á¾ßÇÒ¶§ Ãß°¡ÇÏÀÚ ±ÍÂú´Ù
    m_Positions[POSTIT_PAGE_POSTION_TYPE_A] = { -510.f, 100.f };
    m_Positions[POSTIT_PAGE_POSTION_TYPE_D] = { 510.f, 100.f };
    return S_OK;
}

HRESULT CPostit_Page::Initialize(void* _pArg)
{

    POSTIT_PAGE_DESC* pDesc = static_cast<POSTIT_PAGE_DESC*>(_pArg);


    pDesc->Build_2D_Model(m_pGameInstance->Get_CurLevelID(),
        TEXT("Postit_Page"),
        TEXT("Prototype_Component_Shader_VtxPosTex"),
        (_uint)PASS_VTXPOSTEX::SPRITE2D,
        false);
    pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    m_ePosition = pDesc->eFirstPostion;
    pDesc->Build_2D_Transform(m_Positions[m_ePosition], { 2.2f,2.2f });

    HRESULT hr = __super::Initialize(_pArg);

    if (SUCCEEDED(hr))
    {
        SECTION_MGR->Add_GameObject_ToSectionLayer(pDesc->strInitSkspName, this, SECTION_2D_PLAYMAP_BACKGROUND);
        Set_PlayingAnim(false);
        Set_Render(false);
    }
    Register_OnAnimEndCallBack(bind(&CPostit_Page::Anim_Action_End, this, placeholders::_1, placeholders::_2));


    return hr;

}

void CPostit_Page::Anim_Action(POSTIT_PAGE_ANIM_TYPE eType, _bool _isLoop, POSTIT_PAGE_POSTION_TYPE _ePostionType)
{
    m_eAnimAction = eType;
    switch (eType)
    {
    case Client::CPostit_Page::POSTIT_PAGE_APPEAR:
        START_SFX_DELAY(TEXT("A_sfx_page_disappear"), 0.2f, g_SFXVolume, false);
        Set_Render(true);
        break;
    case Client::CPostit_Page::POSTIT_PAGE_DISAPPEAR:
        START_SFX_DELAY(TEXT("A_sfx_page_disappear"), 0.2f, g_SFXVolume, false);
        break;
    default:
        break;
    }

    if (_ePostionType != m_ePosition)
    {
        m_ePosition = _ePostionType;
        _vector vNewPos = XMLoadFloat2(&m_Positions[m_ePosition]);
        Set_Position(vNewPos);
    }

    Set_PlayingAnim(true);
    Switch_Animation(eType);
    Set_AnimationLoop(COORDINATE_2D, eType, _isLoop);
}

void CPostit_Page::Anim_Action_End(COORDINATE _eCoord, _uint iAnimIdx)
{
    switch (m_eAnimAction)
    {
    case Client::CPostit_Page::POSTIT_PAGE_DISAPPEAR:
        Set_Render(false);
        break;
    default:
        break;
    }
}


CPostit_Page* CPostit_Page::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPostit_Page* pInstance = new CPostit_Page(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CPostit_Page");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPostit_Page::Clone(void* _pArg)
{
    CPostit_Page* pInstance = new CPostit_Page(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CPostit_Page");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPostit_Page::Free()
{
    __super::Free();
}
