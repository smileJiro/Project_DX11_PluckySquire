#include "stdafx.h"
#include "Postit_Page.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CPostit_Page::CPostit_Page(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CPostit_Page::CPostit_Page(const CPostit_Page& _Prototype)
    : CModelObject(_Prototype)
{
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
    //pDesc->Build_2D_Transform({ -510.f, 100.f }, {2.2f,2.2f});

    HRESULT hr = __super::Initialize(_pArg);

    if (SUCCEEDED(hr))
    {
        SECTION_MGR->Add_GameObject_ToSectionLayer(pDesc->strInitSkspName, this, SECTION_2D_PLAYMAP_BACKGROUND);
        Set_PlayingAnim(false);
        Set_Render(false);
    }
    return hr;

}

void CPostit_Page::Anim_Action(POSTIT_PAGE_ANIM_TYPE eType, _bool _isLoop)
{
    Set_PlayingAnim(true);
    Switch_Animation(eType);
    Set_AnimationLoop(COORDINATE_2D, eType, _isLoop);
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
