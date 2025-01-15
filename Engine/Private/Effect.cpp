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


HRESULT CEffect::Initialize_Prototype(const _tchar* _szFilePath)
{
    json jsonEffectInfo;
    if (FAILED(m_pGameInstance->Load_Json(_szFilePath, &jsonEffectInfo)))
        return E_FAIL;

    _int iTemp = jsonEffectInfo["Count"];

    return S_OK;
}

HRESULT CEffect::Initialize(void* _pArg)
{    
    // TODO : EFFECT Desc ? 
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

CEffect* CEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath)
{
    CEffect* pInstance = new CEffect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_szFilePath)))
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
    Safe_Release(m_pParticleBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);

    __super::Free();
}

HRESULT CEffect::Cleanup_DeadReferences()
{
	return S_OK;
}
