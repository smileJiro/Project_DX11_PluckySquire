#pragma once
#include "Base.h"

/* 1. 아빠게임은 찌릿의 대화로 게임이 시작된다. */
/* 2. 아빠게임은 총 7개의 프로그레스가 존재한다. */
/* 3. 각각의 Progress는 클리어 현황을 체크하고, 클리어 현황을 기반으로 조건을 체크하여 수행된다. */
/* 4. 즉 각각의 이벤트에 대한 클리어 현황을 체크하고 이를 기반으로 작업을 수행 */
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CFatherGame_Progress;
class CFatherGame final : public CBase
{
	DECLARE_SINGLETON(CFatherGame)
public:
	enum GAME_STATE { GAME_NONE, GAME_PLAYING, GAME_END, GAME_LAST };
	enum FATHER_PART { FATHER_BODY, FATHER_WING, FATER_HEAD, FATHER_LAST };
	enum FATHER_PROGRESS
	{
		FATHER_PROGRESS_START,			// 고블린과의 전투 후 제트팩 모성의 포탈 디펜더를 여는 것까지.
		FATHER_PROGRESS_ZETPACK,		// 모성에 들어가 찌릿이와 대화하고 나와서 제트팩획득까지.
		FATHER_PROGRESS_PARTBODY,		// 파트 바디 컵에 진입하여 대화하고 나와서 바디 부품을 얻기까지
		FATHER_PROGRESS_PARTWING,		// 파트 윙 컵에서 대화 후, 캔들게임클리어하고 윙 부품을 얻기까지
		FATHER_PROGRESS_PARTHEAD,		// 파트 헤드 컵에서 슈팅게임 클리어하고 헤드 부품을 얻기까지
		FATHER_PROGRESS_MAKEFATHER,		// 아빠 부품을 모두 모아 모성에 돌아가 아빠를 조립하고 나오기까지
		FATHER_PROGRESS_LAST
	};
private:
	CFatherGame();
	virtual ~CFatherGame() = default;

public:
	HRESULT								Start_Game(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // 게임 시작 알림.
	void								Update();
private:
	HRESULT								End_Game(); 

private:
	CGameInstance*						m_pGameInstance = nullptr;
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;

private: /* FatherGame Condition */
	GAME_STATE							m_eGameState = GAME_STATE::GAME_LAST;
	vector<CFatherGame_Progress*>		m_Progress;
	vector<_bool>						m_ProgressClear;
	_uint								m_iClearCount = 0;

private: /* PortalLocker */

private: /* FatherGame UI */
	
public:
	virtual void Free() override;
};

END