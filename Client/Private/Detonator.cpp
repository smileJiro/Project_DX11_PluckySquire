#include "stdafx.h"
#include "Detonator.h"
#include "Bombable.h"

CDetonator::CDetonator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CDetonator::CDetonator(const CDetonator& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CDetonator::Initialize(void* _pArg)
{
    MODELOBJECT_DESC* pBodyDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

    pBodyDesc->eStartCoord = COORDINATE_3D;

    pBodyDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pBodyDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
    pBodyDesc->iRenderGroupID_3D = RG_3D;
    pBodyDesc->iPriorityID_3D = PR3D_GEOMETRY;
    pBodyDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->isCoordChangeEnable = false;
    pBodyDesc->strModelPrototypeTag_3D = TEXT("Detonator");
    return __super::Initialize(_pArg);
}

void CDetonator::Detonate()
{
	if (nullptr == m_pBomb)
		return;
	m_pBomb->Detonate();
    m_pBomb = nullptr;
}

void CDetonator::Set_Bombable(IBombable* _pBomb)
{
    m_pBomb = _pBomb;
}

CDetonator* CDetonator::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDetonator* pInstance = new CDetonator(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : Detonator");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDetonator::Clone(void* _pArg)
{
    CDetonator* pInstance = new CDetonator(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : Detonator");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDetonator::Free()
{
	__super::Free();
}
