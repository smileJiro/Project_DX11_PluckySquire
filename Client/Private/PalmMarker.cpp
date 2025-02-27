#include "stdafx.h"
#include "PalmMarker.h"
#include "GameInstance.h"
#include "Collider_Circle.h"
#include "Player.h"

CPalmMarker::CPalmMarker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CPalmMarker::CPalmMarker(const CPalmMarker& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CPalmMarker::Initialize(void* _pArg)
{
    CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

    pBodyDesc->eStartCoord = COORDINATE_2D;
    pBodyDesc->isCoordChangeEnable = false;

    pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

    pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

    pBodyDesc->eActorType = ACTOR_TYPE::LAST;
    pBodyDesc->pActorDesc = nullptr;
    pBodyDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pBodyDesc->strModelPrototypeTag_2D = TEXT("stamp_icon_stop_01_Sprite");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;



	return S_OK;
}

void CPalmMarker::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CPalmMarker::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CPalmMarker::Render()
{
	return __super::Render();
}



CPalmMarker* CPalmMarker::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPalmMarker* pInstance = new CPalmMarker(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Created PalmMarker Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CPalmMarker::Clone(void* _pArg)
{
    CPalmMarker* pInstance = new CPalmMarker(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Clone PalmMarker Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CPalmMarker::Free()
{
	__super::Free();
}

