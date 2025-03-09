#include "stdafx.h"
#include "FatherPart_Prop.h"
#include "GameInstance.h"

CFatherPart_Prop::CFatherPart_Prop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CFatherPart_Prop::CFatherPart_Prop(const CFatherPart_Prop& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CFatherPart_Prop::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFatherPart_Prop::Initialize(void* _pArg)
{
    return S_OK;
}

void CFatherPart_Prop::Priority_Update(_float _fTimeDelta)
{
}

void CFatherPart_Prop::Update(_float _fTimeDelta)
{
}

void CFatherPart_Prop::Late_Update(_float _fTimeDelta)
{
}

HRESULT CFatherPart_Prop::Render()
{
    return S_OK;
}

CFatherPart_Prop* CFatherPart_Prop::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CFatherPart_Prop* pInstance = new CFatherPart_Prop(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CFatherPart_Prop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFatherPart_Prop::Clone(void* _pArg)
{
	CFatherPart_Prop* pInstance = new CFatherPart_Prop(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CFatherPart_Prop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFatherPart_Prop::Free()
{

	__super::Free();
}

HRESULT CFatherPart_Prop::Cleanup_DeadReferences()
{
    return S_OK;
}
