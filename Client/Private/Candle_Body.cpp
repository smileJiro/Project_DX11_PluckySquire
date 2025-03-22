#include "stdafx.h"
#include "Candle_Body.h"
#include "GameInstance.h"
#include "Effect_Manager.h"

CCandle_Body::CCandle_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CCandle_Body::CCandle_Body(const CCandle_Body& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CCandle_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCandle_Body::Initialize(void* _pArg)
{
    CCandle_Body::MODELOBJECT_DESC* pDesc = static_cast<CCandle_Body::MODELOBJECT_DESC*>(_pArg);

    // Save

    // Add
    pDesc->Build_3D_Model(pDesc->iCurLevelID, TEXT("Candle_01"), TEXT("Prototype_Component_Shader_VtxMesh"), (_uint)PASS_VTXMESH::DEFAULT, false);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->fFrustumCullingRange = 2.0f;
    pDesc->iObjectGroupID = TRIGGER_OBJECT;
    pDesc->iRenderGroupID_3D = RENDERGROUP::RG_3D;
    pDesc->iPriorityID_3D = PR3D_GEOMETRY;
    pDesc->isCoordChangeEnable = false;
    pDesc->isDeepCopyConstBuffer = false;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

	return S_OK;
}

HRESULT CCandle_Body::Ready_Components()
{
	return S_OK;
}

void CCandle_Body::State_Change_Idle()
{
}

void CCandle_Body::State_Change_TurnOn()
{
    // 1. 불 이펙트 재생 시작.
    _vector vPos = Get_FinalPosition();
    _vector vOffsetPos = {0.0f, 1.4f, 0.02f};
    CEffect_Manager::GetInstance()->Active_EffectPosition(TEXT("CandleFire"), true, vPos + vOffsetPos);
    START_SFX_DELAY(TEXT("A_sfx_candle_ignite-") + to_wstring(rand() % 4), 0.f, g_SFXVolume, false);
    START_SFX_DELAY(TEXT("A_sfx_Candle_light_tone"), 0.65f, g_SFXVolume, false);
    //START_SFX_DELAY(TEXT("A_sfx_candle_flame_loop"), 0.f, g_SFXVolume, true);
}

void CCandle_Body::State_Change_FlameLoop()
{
}

void CCandle_Body::State_Change_TurnOff()
{
    // 1. 불 이펙트 재생 끄기. >> 사실 꺼질 일은 없긴 함.
    CEffect_Manager::GetInstance()->InActive_Effect(TEXT("CandleFire"));
    //END_SFX(TEXT("A_sfx_candle_flame_loop"));
    START_SFX_DELAY(TEXT("A_sfx_candle_extinguish"), 0.f, g_SFXVolume, false);
}

CCandle_Body* CCandle_Body::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCandle_Body* pInstance = new CCandle_Body(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CCandle_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CCandle_Body* CCandle_Body::Clone(void* _pArg)
{
    CCandle_Body* pInstance = new CCandle_Body(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCandle_Body");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCandle_Body::Free()
{

    __super::Free();
}
