#include "2DModel.h"
#include "GameInstance.h"

C2DModel::C2DModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModel(_pDevice, _pContext)
{
}

C2DModel::C2DModel(const C2DModel& _Prototype)
	:CModel(_Prototype)
{
}


HRESULT C2DModel::Initialize(void* _pDesc)
{
    /* Com_VIBuffer */
    CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, 1, TEXT("Prototype_Component_VIBuffer_Rect"), nullptr));
    if (nullptr == pComponent)
        return E_FAIL;

    m_pVIBufferCom = static_cast<CVIBuffer_Rect*>(pComponent);

	return S_OK;
}

void C2DModel::Set_AnimationLoop(_uint iIdx, _bool bIsLoop)
{

}

void C2DModel::Set_Animation(_uint iIdx)
{
}

void C2DModel::Switch_Animation(_uint iIdx)
{
}


HRESULT C2DModel::Render(CShader* _Shader, _uint _iShaderPass)
{

	if (m_iSRVIndex != UINT_MAX)
	{
		if(_Shader)
			_Shader->Begin(0);
		if (m_pVIBufferCom)
		{
			m_pVIBufferCom->Bind_BufferDesc();
			m_pVIBufferCom->Render();
		}
	}

	return S_OK;
}

_bool C2DModel::Play_Animation(_float fTimeDelta)
{
	return _bool();
}


C2DModel* C2DModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DModel* pInstance = new C2DModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : C2DModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* C2DModel::Clone(void* _pArg)
{
	C2DModel* pInstance = new C2DModel(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : C2DModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DModel::Free()
{
	__super::Free();
}
