#include "2DModel.h"
#include "GameInstance.h"

HRESULT C2DModel::Initialize(void* _pDesc)
{
    /* Com_VIBuffer */
    CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, 1, TEXT("Prototype_Component_VIBuffer_Rect"), nullptr));
    if (nullptr == pComponent)
        return E_FAIL;

    m_pVIBufferCom = static_cast<CVIBuffer_Rect*>(pComponent);

	return S_OK;
}

HRESULT C2DModel::Render(_uint iMeshIndex)
{
	return E_NOTIMPL;
}
