#include "stdafx.h"
#include "MapObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"


CMapObject::CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMapObject::CMapObject(const CMapObject& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMapObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMapObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_DESC* pDesc = static_cast<CMapObject::MAPOBJ_DESC*>(_pArg);
    pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;

    return __super::Initialize(_pArg);
}


void CMapObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void CMapObject::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CMapObject::Render()
{
    return __super::Render();
}


void CMapObject::Free()
{
    __super::Free();
}
