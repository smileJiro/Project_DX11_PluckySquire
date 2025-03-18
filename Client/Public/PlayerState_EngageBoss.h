#pragma once
#include "PlayerState.h"
BEGIN(Client)
class CPlayerState_EngageBoss :
    public CPlayerState
{
public:

    CPlayerState_EngageBoss(CPlayer* _pOwner);

    // CPlayerState을(를) 통해 상속됨
    void Update(_float _fTimeDelta) override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) override;

private:

};

END


//컵 속으로 들어가면 시네마틱 시작.
//찌릿에게 접근,
//찌릿과 대화
//조트 변신(ANIMATION_2D::CyberjotLite_Transform) , 찌릿 합체
//시네마틱 종료
//플레이어가 조작해서 포탈을 타고 컵에서 나옴.
//나오는 순간 시네마틱 또 시작.
//Cyberjot_cine_LB_engage_Sh01 재생.
//잠깐 암전.(FadeOut)
//보스 앞에서 Cyberjot_cine_LB_engage_Sh02 재생. 
//보스 소리지르기  (카메라 위치 : 1.74 , 25.2, 5.42)
//보스전 시작