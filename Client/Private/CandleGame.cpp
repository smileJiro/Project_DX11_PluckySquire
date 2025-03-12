#include "stdafx.h"
#include "CandleGame.h"
#include "GameInstance.h"
#include "Candle.h"

CCandleGame::CCandleGame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject(_pDevice, _pContext)
{
}

CCandleGame::CCandleGame(const CCandleGame& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CCandleGame::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCandleGame::Initialize(void* _pArg)
{
    CCandleGame::CANDLEGAME_DESC* pDesc = static_cast<CCandleGame::CANDLEGAME_DESC*>(_pArg);
    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    pDesc->iObjectGroupID = NONE;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Candles(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CCandleGame::Priority_Update(_float _fTimeDelta)
{
    Check_Clear(_fTimeDelta);
}

void CCandleGame::OnOffCandleUIs(_bool _isOnOff)
{
    for (auto& pCandle : m_Candles)
    {
        pCandle->OnOff_Candle_UI(_isOnOff);
    }
}

void CCandleGame::Check_Clear(_float _fTimeDelta)
{
    if (true == m_isClearGame)
        return;


    _int iFlameCount = 0;
    m_vClearCheckCycleTime.y += _fTimeDelta;

    if (m_vClearCheckCycleTime.x <= m_vClearCheckCycleTime.y)
    {
        m_vClearCheckCycleTime.y = 0.0f;
        /* 1. Check 주기가 돌았을때. 검사를 수행한다. 매프레임 하는 것도 손해 */
        for (auto& pCandle : m_Candles)
        {
            if (CCandle::STATE_TURNON == pCandle->Get_CurState())
            {
                /* 1. 만약 Candle State 가 FalmeLoop 라면, 카운트증가. */
                ++iFlameCount;
            }
        }

    }

    if (m_Candles.size() == iFlameCount)
    {

        ClearGame();
    }
}

void CCandleGame::ClearGame()
{
    /* 캔들의 개수와 Flame Count가 같다면, 클리어 한 것을 본다. */
    m_isClearGame = true;
    m_vClearCheckCycleTime.y = 0.0f;

#ifdef _DEBUG
    cout << "Clear Game" << endl;
#endif // _DEBUG


}

HRESULT CCandleGame::Ready_Candles(CANDLEGAME_DESC* _pDesc)
{
    _uint iNumCandles = (_uint)_pDesc->CandlePositions.size();
    m_Candles.resize(iNumCandles);

    /* 1. 캔들 객체 생성. */

    _float fModelSizeX = 100.f; // 추측 픽셀 사이즈
    _float fStartX = (g_iWinSizeX - fModelSizeX * iNumCandles) * 0.5f + fModelSizeX * 0.5f;

    for (_uint i = 0; i < iNumCandles; ++i) //
    {
        /* Test Candle */
        CCandle::CANDLE_DESC CandleDesc;
        CandleDesc.iCurLevelID = m_iCurLevelID;
        CandleDesc.Build_3D_Transform(_pDesc->CandlePositions[i]);
        CandleDesc.vCandleUIDesc.x = fStartX + fModelSizeX * i;
        CandleDesc.vCandleUIDesc.y = g_iWinSizeY * 0.9f;
        CandleDesc.vCandleUIDesc.z = 0.75f;
        CandleDesc.vCandleUIDesc.w = 0.75f;
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(m_iCurLevelID, TEXT("Prototype_GameObject_Candle"), m_iCurLevelID, TEXT("Layer_Candle"), &pGameObject, &CandleDesc)))
            return E_FAIL;
        m_Candles[i] = (static_cast<CCandle*>(pGameObject));
        Safe_AddRef(pGameObject);
    }

    //UIDesc.fX = g_iWinSizeX * 0.5f;
    //UIDesc.fY = g_iWinSizeY * 0.9f;
    //UIDesc.fSizeX = 0.75f;
    //UIDesc.fSizeY = 0.75f;
    return S_OK;
}

CCandleGame* CCandleGame::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCandleGame* pInstance = new CCandleGame(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : CCandleGame");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CCandleGame* CCandleGame::Clone(void* _pArg)
{
    CCandleGame* pInstance = new CCandleGame(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CCandleGame");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCandleGame::Free()
{
    for (auto& pCandle : m_Candles)
        Safe_Release(pCandle);
    m_Candles.clear();

    __super::Free();
}

HRESULT CCandleGame::Cleanup_DeadReferences()
{
    return S_OK;
}
