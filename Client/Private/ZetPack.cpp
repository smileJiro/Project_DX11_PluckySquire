#include "stdafx.h"
#include "ZetPack.h"
#include "GameInstance.h"
#include "Player.h"
#include "Actor_Dynamic.h"

CZetPack::CZetPack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CZetPack::CZetPack(const CZetPack& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CZetPack::Initialize(void* _pArg)
{
	ZETPACK_DESC* pDesc = (ZETPACK_DESC*)_pArg;
	m_pPlayer = pDesc->pPlayer;
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
    pDesc->strModelPrototypeTag_3D = TEXT("Zip_rig");
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
	return S_OK;
}

void CZetPack::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CZetPack::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CZetPack::Render()
{
	return __super::Render();
}

void CZetPack::ReFuel()
{
    m_fFuel = m_fMaxFuel;
}

//»ó½ÂÇÏ±â
void CZetPack::Propel(_float _fTimeDelta)
{
	m_fFuel -= m_fFuelConsumption * _fTimeDelta;
    if (m_fFuel >= 0.f)
        m_pPlayer->Add_Force(_vector{ 0.f,m_fPropelForce * m_fFuel / m_fMaxFuel *_fTimeDelta,0.f });
    else
    {
        int a = 0;
    }
    cout << "Fuel" << m_fFuel << endl;
}

void CZetPack::Retropropulsion()
{
    CActor_Dynamic* pActor = m_pPlayer->Get_ActorDynamic();
    _vector vVelodicty = pActor->Get_LinearVelocity();
    _float fVelocityY = XMVectorGetY(vVelodicty);
    if (fVelocityY < 0.f)
        fVelocityY *= 0.5f;
	pActor->Set_LinearVelocity(XMVectorSetY(vVelodicty, fVelocityY));
}

CZetPack* CZetPack::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CZetPack* pInstance = new CZetPack(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : ZetPack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CZetPack::Clone(void* _pArg)
{
    CZetPack* pInstance = new CZetPack(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : ZetPack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CZetPack::Free()
{
    __super::Free();
}
