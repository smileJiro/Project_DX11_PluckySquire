#include "stdafx.h"
#include "FallingRock.h"
#include "GameInstance.h"

CFallingRock::CFallingRock(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CFallingRock::CFallingRock(const CFallingRock& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CFallingRock::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFallingRock::Initialize(void* _pArg)
{
	//CFallingRock::model
	return S_OK;
}

void CFallingRock::Priority_Update(_float _fTimeDelta)
{
}

void CFallingRock::Update(_float fTimeDelta)
{
}

void CFallingRock::Late_Update(_float _fTimeDelta)
{
}

HRESULT CFallingRock::Render()
{
	return E_NOTIMPL;
}

CFallingRock* CFallingRock::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CFallingRock::Clone(void* _pArg)
{
	return nullptr;
}

void CFallingRock::Free()
{
}
