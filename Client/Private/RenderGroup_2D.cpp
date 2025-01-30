#include "stdafx.h"
#include "RenderGroup_2D.h"
#include "GameInstance.h"

CRenderGroup_2D::CRenderGroup_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CRenderGroup_MRT(_pDevice, _pContext)
{
}

HRESULT CRenderGroup_2D::Add_RenderObject(CGameObject* _pGameObject)
{
    if (COORDINATE_3D == _pGameObject->Get_CurCoord())
        return S_OK;
    return __super::Add_RenderObject(_pGameObject);
}

CRenderGroup_2D* CRenderGroup_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CRenderGroup_2D* pInstance = new CRenderGroup_2D(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Create Failed CRenderGroup_2D");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CRenderGroup_2D::Free()
{

    __super::Free();
}
