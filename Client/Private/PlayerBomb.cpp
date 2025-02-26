#include "stdafx.h"
#include "PlayerBomb.h"
#include "GameInstance.h"
#include "Player.h"
#include "Section_Manager.h"

CPlayerBomb::CPlayerBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CPlayerBomb::CPlayerBomb(const CPlayerBomb& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CPlayerBomb::Initialize(void* _pArg)
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
    pBodyDesc->strModelPrototypeTag_2D = TEXT("playerbomb");

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;
    Switch_Animation(IDLE);

    return S_OK;
}

void CPlayerBomb::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CPlayerBomb::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CPlayerBomb::Render()
{
    return __super::Render();
}

CPlayerBomb* CPlayerBomb::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerBomb* pInstance = new CPlayerBomb(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PlayerBomb");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerBomb::Clone(void* _pArg)
{
    CPlayerBomb* pInstance = new CPlayerBomb(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : PlayerBomb");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerBomb::Free()
{
	__super::Free();
}
