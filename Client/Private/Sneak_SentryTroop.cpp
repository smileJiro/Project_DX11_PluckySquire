#include "stdafx.h"
#include "Sneak_SentryTroop.h"
#include "GameInstance.h"

CSneak_SentryTroop::CSneak_SentryTroop(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CSneak_Troop(_pDevice, _pContext)
{
}

CSneak_SentryTroop::CSneak_SentryTroop(const CSneak_SentryTroop& _Prototype)
	: CSneak_Troop(_Prototype)
{
}

HRESULT CSneak_SentryTroop::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CSneak_SentryTroop::Initialize(void* _pArg)
{
	return E_NOTIMPL;
}

void CSneak_SentryTroop::Priority_Update(_float _fTimeDelta)
{
}

void CSneak_SentryTroop::Update(_float _fTimeDelta)
{
}

void CSneak_SentryTroop::Late_Update(_float _fTimeDelta)
{
}

HRESULT CSneak_SentryTroop::Render()
{
	return E_NOTIMPL;
}
