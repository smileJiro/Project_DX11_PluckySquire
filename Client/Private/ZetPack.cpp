#include "stdafx.h"
#include "ZetPack.h"
#include "GameInstance.h"
#include "Player.h"
#include "Actor_Dynamic.h"
#include "Effect_Manager.h"


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

    if (FAILED(Ready_TargetLight()))
        return E_FAIL;

	return S_OK;
}

void CZetPack::Priority_Update(_float _fTimeDelta)
{
    m_bPropel = false;
    __super::Priority_Update(_fTimeDelta);
}

void CZetPack::Update(_float _fTimeDelta)
{

	__super::Update(_fTimeDelta);


}

void CZetPack::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
    if (m_bPropel)
    {
        _float fPlayerUpForce = m_pPlayer->Get_UpForce();
        if (0.f > fPlayerUpForce)
            Switch_State(ZET_STATE::STATE_DESCEND);
        else if (0.f < fPlayerUpForce)
            Switch_State(ZET_STATE::STATE_ASCEND);
    }
    else
        Switch_State(ZET_STATE::STATE_IDLE);
}

HRESULT CZetPack::Render()
{
 //   cout << m_pControllerModel->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex() << endl;
	return __super::Render();
}

void CZetPack::ReFuel()
{
    m_fFuel = m_fMaxFuel;
}

//상승하기
void CZetPack::Propel(_float _fTimeDelta)
{
    m_bPropel = true;
    COORDINATE eCoord = Get_CurCoord();
    _float fFuelConsumpTion = COORDINATE_2D == eCoord ? m_fFuelConsumption2D : m_fFuelConsumption3D;
	_float fMinForceRatio = COORDINATE_2D == eCoord ? m_fMinForeceRatio2D : m_fMinForeceRatio3D;
    _float fMinForceFuel = m_fMaxFuel * (COORDINATE_2D == eCoord ? m_fMinForceFuelRatio2D : m_fMinForceFuelRatio3D);
	m_fFuel -= fFuelConsumpTion * _fTimeDelta ;
    if (m_fFuel >= 0.f)
    {
        _float fRatio = fMinForceRatio + (m_fMaxForeceRatio - fMinForceRatio) * max(m_fFuel - fMinForceFuel, 0.f) / m_fMaxFuel;
        //cout << "Ratio" << fRatio << endl;
        m_pPlayer->Add_Upforce((COORDINATE_2D == eCoord ? m_fPropelForce2D : m_fPropelForce3D)
            * fRatio * _fTimeDelta);
    }
    else
    {
        m_fFuel = 0.f;

    }
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
        {
            m_pControllerModel->Switch_Animation(ANIM_IDLE);
        }
        else
        {
            CEffect_Manager::GetInstance()->Active_EffectID(TEXT("Zip"), true, &m_WorldMatrices[COORDINATE_3D], 0);
            CEffect_Manager::GetInstance()->Stop_SpawnID(TEXT("Zip"), 1.f, 1);
            Event_SetActive(m_pTargetLight, false);
        }
        break;
	case STATE_ASCEND:
        if (COORDINATE_2D == eCoord)
        {
		    m_pControllerModel->Switch_Animation(ANIM_ASCEND);
        }
        else
        {
            CEffect_Manager::GetInstance()->Active_EffectID(TEXT("Zip"), false, &m_WorldMatrices[COORDINATE_3D], 1);
            Event_SetActive(m_pTargetLight, true);
            
        }
		break;
	case STATE_DESCEND:
        if (COORDINATE_2D == eCoord)
        {
		    m_pControllerModel->Switch_Animation(ANIM_DESCEND);
        }
        else
        {
            CEffect_Manager::GetInstance()->Active_EffectID(TEXT("Zip"), false, &m_WorldMatrices[COORDINATE_3D], 1);
            Event_SetActive(m_pTargetLight, true);
        }
		break;
	default:
		break;
	}
    m_eState = _eState;
}

HRESULT CZetPack::Ready_TargetLight()
{
    ///* 점광원 */
    CONST_LIGHT LightDesc = {};
    LightDesc.vPosition = _float3(0.0f, 0.0f, 0.0f);
    LightDesc.fFallOutStart = 5.2f;
    LightDesc.fFallOutEnd = 13.0f;
    LightDesc.vRadiance = _float3(9.0f, 9.0f, 9.0f);
    LightDesc.vDiffuse = _float4(1.0f, 0.371f, 0.0f, 1.0f);
    LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.0f);
    LightDesc.vSpecular = _float4(1.0f, 0.450f, 0.0f, 1.0f);

    if (FAILED(m_pGameInstance->Add_Light_Target(LightDesc, LIGHT_TYPE::POINT, this, _float3(0.0f, 0.0f, 0.0f), &m_pTargetLight, true)))
        return E_FAIL;

    Safe_AddRef(m_pTargetLight);
    m_pTargetLight->Set_Active(false);
    return S_OK;
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
    Safe_Release(m_pTargetLight); // 순환참조로 인해플레이어쪽에서만 Ref 카운트 관리.

    __super::Free();
}
