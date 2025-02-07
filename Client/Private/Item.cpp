#include "stdafx.h"
#include "Item.h"

CItem::CItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CItem::CItem(const CItem& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CItem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem::Initialize(void* _pArg)
{
	return S_OK;
}

void CItem::Priority_Update(_float _fTimeDelta)
{
}

void CItem::Update(_float _fTimeDelta)
{
}

void CItem::Late_Update(_float _fTimeDelta)
{
}

void CItem::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CItem::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CItem::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CItem::Free()
{
	__super::Free();
}
