#include "stdafx.h"
#include "Excavator_Switch.h"
#include "GameInstance.h"

CExcavator_Switch::CExcavator_Switch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CExcavator_Switch::CExcavator_Switch(const CExcavator_Switch& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CExcavator_Switch::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CExcavator_Switch::Initialize(void* _pArg)
{
    return S_OK;
}

void CExcavator_Switch::Priority_Update(_float _fTimeDelta)
{
}

void CExcavator_Switch::Update(_float _fTimeDelta)
{
}

void CExcavator_Switch::Late_Update(_float _fTimeDelta)
{
}

void CExcavator_Switch::State_Change()
{
}

void CExcavator_Switch::State_Change_Lock()
{
}

void CExcavator_Switch::State_Change_Open()
{
}

void CExcavator_Switch::Action_State(_float _fTimeDelta)
{
}

void CExcavator_Switch::Action_State_Lock(_float _fTimeDelta)
{
}

void CExcavator_Switch::Action_State_Open(_float _fTimeDelta)
{
}

HRESULT CExcavator_Switch::Ready_Components(SWITCH_DESC* _pDesc)
{
    return E_NOTIMPL;
}

CExcavator_Switch* CExcavator_Switch::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CExcavator_Switch::Clone(void* _pArg)
{
    return nullptr;
}

void CExcavator_Switch::Free()
{
}
