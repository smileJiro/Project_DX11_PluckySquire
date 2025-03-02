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
    pDesc->isCoordChangeEnable = true;

    pDesc->iModelPrototypeLevelID_3D = LEVEL_STATIC;
    pDesc->strModelPrototypeTag_3D = TEXT("Zip_rig");
    pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;
    pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;

    pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
    pDesc->strModelPrototypeTag_2D = TEXT("zetpack");
    pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
    pDesc->tTransform2DDesc.vInitialPosition = { 0.f,0.f,0.f };
    pDesc->tTransform2DDesc.vInitialScaling = { 1.f,1.f,1.f };
    pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
    pDesc->iObjectGroupID = OBJECT_GROUP::PLAYER;
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;
	return S_OK;
}

void CZetPack::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);


    _float fPlayerUpForce = m_pPlayer->Get_UpForce();
    if (fPlayerUpForce <= 0.f)
        Switch_State(STATE_IDLE);
}

void CZetPack::Late_Update(_float _fTimeDelta)
{
    //E_DIRECTION eDir = m_pPlayer->Get_2DDirection();;
    //switch (eDir)
    //{
    //case Client::E_DIRECTION::LEFT:
    //case Client::E_DIRECTION::LEFT_UP:
    //case Client::E_DIRECTION::LEFT_DOWN:
    //{
    //    _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
    //    Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
    //    break;
    //}
    //case Client::E_DIRECTION::RIGHT:
    //case Client::E_DIRECTION::RIGHT_UP:
    //case Client::E_DIRECTION::RIGHT_DOWN:
    //{
    //    _vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
    //    Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
    //    break;
    //}
    //default:
    //    break;
    //}
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
    COORDINATE eCoord = Get_CurCoord();
    _float fFuelConsumpTion = COORDINATE_2D == eCoord ? m_fFuelConsumption2D : m_fFuelConsumption3D;
	_float fMinForceRatio = COORDINATE_2D == eCoord ? m_fMinForeceRatio2D : m_fMinForeceRatio3D;
    _float fMinForceFuel = m_fMaxFuel* COORDINATE_2D == eCoord ? m_fMinForceFuelRatio2D : m_fMinForceFuelRatio3D;
	m_fFuel -= fFuelConsumpTion * _fTimeDelta ;
    if (m_fFuel >= 0.f)
    {
        _float fRatio = fMinForceRatio + (m_fMaxForeceRatio - fMinForceRatio) * max(m_fFuel - fMinForceFuel, 0.f) / m_fMaxFuel;
        m_pPlayer->Add_Upforce(COORDINATE_2D == eCoord ? m_fPropelForce2D : m_fPropelForce3D 
            * fRatio * _fTimeDelta);

        _float fPlayerUpForce = m_pPlayer->Get_UpForce();
        if (0.f > fPlayerUpForce)
            Switch_State(ZET_STATE::STATE_DESCEND);
        else if (0.f < fPlayerUpForce)
            Switch_State(ZET_STATE::STATE_ASCEND);
    }
    else
    {
        m_fFuel = 0.f;
        Switch_State(ZET_STATE::STATE_IDLE);
    }
    //cout << "m_fFuel : " << m_fFuel << endl;


}


void CZetPack::Switch_State(ZET_STATE _eState)
{
	if (_eState == m_eState)
		return;
	COORDINATE eCoord = Get_CurCoord();
	switch (_eState)
	{
	case STATE_IDLE:
        if (COORDINATE_2D == eCoord)
            m_pControllerModel->Switch_Animation(ANIM_IDLE);
        break;
	case STATE_ASCEND:
        if(COORDINATE_2D == eCoord)
		    m_pControllerModel->Switch_Animation(ANIM_ASCEND);
		break;
	case STATE_DESCEND:
        if (COORDINATE_2D == eCoord)
		    m_pControllerModel->Switch_Animation(ANIM_DESCEND);
		break;
	default:
		break;
	}
    m_eState = _eState;
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
