#include "Effect.h"
#include "GameInstance.h"


CEffect::CEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CGameObject(_pDevice, _pContext)
{
}

CEffect::CEffect(const CEffect& _Prototype)
	: CGameObject(_Prototype)
{
}

HRESULT CEffect::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect::Initialize(void* _pArg)
{
    // TODO : 2D or 3D ?

    {
        EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(_pArg);
        if (nullptr == pDesc)
            return E_FAIL;

        pDesc->eStartCoord = COORDINATE_3D; 
        pDesc->isCoordChangeEnable = false;
    }


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;



    return S_OK;
}

void CEffect::Priority_Update(_float _fTimeDelta)
{
}

void CEffect::Update(_float _fTimeDelta)
{
}

void CEffect::Late_Update(_float _fTimeDelta)
{
}

HRESULT CEffect::Render()
{
    return S_OK;
}

CEffect* CEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CEffect* pInstance = new CEffect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Cloned : CEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect::Clone(void* _pArg)
{
    CEffect* pInstance = new CEffect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect::Free()
{
    __super::Free();
}

HRESULT CEffect::Cleanup_DeadReferences()
{
	return S_OK;
}
