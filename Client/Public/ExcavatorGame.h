#pragma once
#include "Base.h"

BEGIN(Engine)
class CCharacter;
class CGameInstance;
END
BEGIN(Client)
class CExcavator_Tread;
class CExcavator_Centre;

class CExcavatorGame : public CBase
{
    DECLARE_SINGLETON(CExcavatorGame)

public:
    enum EXCAVATOR_PART { EXCAVATOR_CENTRE, EXCAVATOR_TREAD_L, EXCAVATOR_TREAD_R, EXCAVATOR_SAW, EXCAVATOR_LAST };
    // 첫 시작, 첫 전투 대기, 톱니 가동, 맞을때, 죽을때
    enum EXCAVATOR_STATE { STATE_MOVE_R, STATE_IDLE, STATE_SAW, STATE_HIT, STATE_DEAD, STATE_LAST };
    enum PROGRESS 
    {
        PROGRESS_1, // EXCAVATOR MOVE_R >> EXCAVATOR_IDLE >> DOORCLOSE >> THRASH TALK >> MONSTER 2 BATTLE >> DOOR OPEN ( SWITCH 가 2일때 PROGRESS 2로 전환.)
        PROGRESS_2, // DOORCLOSE >> MONSTER 2 BATTLE >> DOOR OPEN >> SWITCH 실패 카운트 검사 >> 문다시 닫힘 (SWITCH 실패 카운트1 일때 PROGRESS3으로 전환)
        PROGRESS_3, // DOORCLOSE >> MONSTER 2 BATTLE >> DOOR OPEN >> THRASH TALK >> PORTAL ON >> TURRET ON >> STOPPABLE >> THRASH PULL ( SWITCH 1 일때 PROGRESS 4)
        PROGRESS_4, // DOORCLOSE >> MONSTER 2 BATTLE >> DOOR OPEN >> STOPPABLE >> THRASH PULL >> ATTACK REGULATOR >> EXCAVATOR STATE DEAD (DEAD일때 프로그래스 종료)
        PROGRESS_LAST, // DOORCLOSE >> MONSTER 2 BATTLE >> DOOR OPEN >> STOPPABLE >> THRASH PULL >> ATTACK REGULATOR >> EXCAVATOR STATE DEAD (DEAD일때 프로그래스 종료)
    };
private:
    CExcavatorGame();
    virtual ~CExcavatorGame() = default;

public:
    HRESULT Start_Game(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    void    Update(_float _fTimeDelta);
    
private:
    _bool   Update_Progress(_float _fTimeDelta);
    _bool    Update_Progress_1(_float _fTimeDelta);
    _bool    Update_Progress_2(_float _fTimeDelta);
    _bool    Update_Progress_3(_float _fTimeDelta);
    _bool    Update_Progress_4(_float _fTimeDelta);

    void    Enter_Progress();
    void    Exit_Progress();
    void    Game_End();
private:
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    CGameInstance* m_pGameInstance = nullptr;

private:
    _bool                   m_isPlaying = false;
    _bool                   m_isGameEnd = false;
    _uint                   m_iCurProgress = (_uint)PROGRESS::PROGRESS_1;
    _uint                   m_iSwitchCount = 0;
    vector<CCharacter*>     m_ExcavatorParts{};
    vector<class CDoor_Red*> m_Doors;
private: /* Excavator State */
    EXCAVATOR_STATE m_ePreState = EXCAVATOR_STATE::STATE_LAST;
    EXCAVATOR_STATE m_eCurState = EXCAVATOR_STATE::STATE_LAST;

private: /* Move_R */
    _float2 m_vMoveTime = { 1.0f, 0.0f };
    _float2 m_vMoveSpeed = { 1000.f, 0.0f };

private:
    HRESULT Ready_ExcavatorParts();

private:
    void State_Change();
    void State_Change_Move_R();
    void State_Change_Idle();
    void State_Change_Saw();
    void State_Change_Hit();
    void State_Change_Dead();

private:
    void Action_State(_float _fTimeDelta);
    void Action_State_Move_R(_float _fTimeDelta);
    void Action_State_Idle(_float _fTimeDelta);
    void Action_State_Saw(_float _fTimeDelta);
    void Action_State_Hit(_float _fTimeDelta);
    void Action_State_Dead(_float _fTimeDelta);

public:
    void Free() override;
};

END