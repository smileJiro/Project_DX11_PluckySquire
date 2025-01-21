#include "Actor_Dynamic.h"

CActor_Dynamic::CActor_Dynamic(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ACTOR_TYPE _eActorType)
    :CActor(_pDevice, _pContext, _eActorType)
{
}

CActor_Dynamic::CActor_Dynamic(const CActor_Dynamic& _Prototype)
    :CActor(_Prototype)
{
}

HRESULT CActor_Dynamic::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CActor_Dynamic::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

    return S_OK;
}

void CActor_Dynamic::Priority_Update(_float _fTimeDelta)
{
}

void CActor_Dynamic::Update(_float _fTimeDelta)
{
}

void CActor_Dynamic::Late_Update(_float _fTimeDelta)
{
}


CActor_Dynamic* CActor_Dynamic::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _bool _isKinematic)
{
	ACTOR_TYPE eActorType = ACTOR_TYPE::DYNAMIC;
	if (true == _isKinematic)
		eActorType = ACTOR_TYPE::KINEMATIC;

	CActor_Dynamic* pInstance = new CActor_Dynamic(_pDevice, _pContext, eActorType);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CActor_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}


CComponent* CActor_Dynamic::Clone(void* _pArg)
{
	CActor_Dynamic* pInstance = new CActor_Dynamic(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CActor_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_Dynamic::Free()
{

	__super::Free();
}
