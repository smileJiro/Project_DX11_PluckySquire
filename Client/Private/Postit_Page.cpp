#include "stdafx.h"
#include "Postit_Page.h"

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
    Set_PlayingAnim(false);
    return __super::Initialize(_pArg);
}

void CPostit_Page::Anim_Action(POSTIT_PAGE_ANIM_TYPE eType, _bool _isLoop)
{
    Set_PlayingAnim(true);
    Set_Animation(eType);
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
