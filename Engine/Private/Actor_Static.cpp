#include "Actor_Static.h"

CActor_Static::CActor_Static(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CActor(_pDevice, _pContext, ACTOR_TYPE::STATIC)
{
}

CActor_Static::CActor_Static(const CActor_Static& _Prototype)
	:CActor(_Prototype)
{
}

HRESULT CActor_Static::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

    return S_OK;
}

HRESULT CActor_Static::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

    return S_OK;
}

void CActor_Static::Priority_Update(_float _fTimeDelta)
{
}

void CActor_Static::Update(_float _fTimeDelta)
{
}

void CActor_Static::Late_Update(_float _fTimeDelta)
{
#ifdef _DEBUG
	CActor::Late_Update(_fTimeDelta); // Debug_Render (Trigger Shape)
#endif // _DEBUG
}


CActor_Static* CActor_Static::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CActor_Static* pInstance = new CActor_Static(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CActor_Static");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CActor_Static::Clone(void* _pArg)
{
	CActor_Static* pInstance = new CActor_Static(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CActor_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_Static::Free()
{

	__super::Free();
}
