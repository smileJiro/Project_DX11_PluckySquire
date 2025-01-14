#pragma once
#include "Base.h"

/* 1. 현재 레이어에 있는 부모 ui를 순회하며, 현재 마우스가 올라간 최 하단 자식 ui를 찾는다. */
/* 2. 1차적인 겹쳐지는 ui를 고려하여 ui의 depth에 따라 정렬 후, 1번을 수행하여도 될 것 같다. */
/* 3. 마우스의 상태에 따라 Target UI에게 필요한 함수를 호출 한다.*/

BEGIN(Engine)
class CGameInstance;
class CUI;

class CUI_Manager final : public CBase
{
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT			Initialize();
	void			Update();
public:
	void			Initialize_UIDepth(); // 레벨 변경 시, UIDepth 초기화.(값 할당.)
	void			Change_FocusedUI(CUI* _pNewFocusedUI); // FocusedUI를 변경.
	void			Clear();
public:
	// Get
	
	// Set
	void			Set_CurLevelIndex(_uint _iCurLevelIndex) { m_iCurLevelIndex = _iCurLevelIndex; } // 현재 인덱스를 기억하고 해당 인덱스에 접근해야함.
private:
	CGameInstance*	m_pGameInstance = nullptr;
	CUI*			m_pFocusedUI = nullptr;		// Focus 중인 부모 UI를 지칭. 
	CUI*			m_pTargetedUI = nullptr;	// 현재 타게팅 된 UI를 기억하다가, 타겟팅이 변경된다면 MouseAway()를 호출.
	_uint			m_iMaxDepth = 0;
	_uint			m_iCurLevelIndex = 0;		// 현재 플레이중인 인덱스에 대한...

private:
	CUI*			Search_FocusedUI();
	void			ExecuteStateOfTargetedUI_AllUI(); // focused 무관하게 상호작용하는 함수.(범용성 상승)
	void			ExecuteStateOfTargetedUI_FocusedUI(); // focused만 상호작용하는 함수 (최적화 상승)
	CUI*			Search_TargetedUI(CUI* _pParentUI);

public:
	static CUI_Manager* Create();
	virtual void Free() override;
};

END