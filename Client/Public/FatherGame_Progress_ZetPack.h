#pragma once
#include "FatherGame_Progress.h"

/* 1. Enter : 고블린 3마리 생성, PortalDefender 4곳에 생성 */
/* 2. Update : 고블린 처치 수 체크 */
/* 3. ClearCheck : 고블린 3마리 모두 처치 시 */
/* 4. Clear : Progress_Start_Clear Event 발생, Off */
/* 5. Exit : Progress_ZetPack On */

BEGIN(Client)
class CMonster;
class CPortalLocker;
class CZetPack_Child;
class CFatherGame_Progress_ZetPack : public CFatherGame_Progress
{
public:
	typedef struct tagFatherGameProgress_ZetPack
	{

	}FATHERGAME_PROGRESS_ZETPACK_DESC;

private:
	CFatherGame_Progress_ZetPack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CFatherGame_Progress_ZetPack() = default;

public:
	virtual HRESULT			Initialize(void* _pArg);
	HRESULT					Progress_Enter() override;		// Active OnEnable 에서 호출.
	void					Progress_Update() override;		
	HRESULT					Progress_Clear() override;		// Clear 조건 검사 + Clear 시 동작 정의
	HRESULT					Progress_Exit() override;		// Active OnDisable 에서 호출.

private:
	CZetPack_Child*			m_pZetPack_Child = nullptr;

public:
	static CFatherGame_Progress_ZetPack* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg);
	void					Free() override;
};

END