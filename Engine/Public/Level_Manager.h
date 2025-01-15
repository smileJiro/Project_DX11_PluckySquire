#pragma once
#include "Base.h"
// Manager Class 라고 하더라도, 싱글톤으로 작성하지 않는다.
// GameInstance 객체만을 가지고 Engine에 존재하는 모든 Manager Class에 접근 할거기 때문
// Engine_dll 역시 필요가 없겠지, 클라이언트에 공개할게 아니기때문

BEGIN(Engine)

// 전방선언을 여기서 하나, 멤버변수 앞에서 class를 붙이나 동작 자체나 성능은 차이가 없다.
// 본인 스타일에 맞게 작성하면 될 것 같다.
class CLevel;
class CGameInstance;


// Level Manager의 역할.
// 1. 현재 화면에 보여주어야 할 레벨 객체를 들고 있는다.
// 2. 새로운 레벨로 교체하고 기존 레벨을 삭제한다.
// 2_1. 기존 레벨용 자원(리소스, 게임 오브젝트)들을 삭제한다.
// 3. 활성화된 레벨의 반복적인 업데이트를 수행한다.

class CLevel_Manager final : public CBase // final : 해당 클래스에 대한 상속 금지
{
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	HRESULT			Level_Enter(_int _iLevelID, CLevel* _pNewLevel); // Level 진입 시 해야 할 처리.
	HRESULT			Level_Exit(); //  Level 탈출 시 해야 할 처리.

	void			Update(_float _fTimeDelta);
	HRESULT			Render(); // 역시나 큰 의미 없겠지, Renderer에서 실제 렌더 코드를 작성할테니.
public:
	_int			Get_CurLevelID() const { return m_iCurLevelID;}
private:
	// CurLevelID는 Client에 의존적이지 않으면서, 해당 Level들에 대한 관리를 하는 용도이다.
	// 그렇기에 어떠한 Client에서 사용될 지 모르지만, 대부분의 게임의 Scene들은 Enum을 통해 관리하니.
	// -1을 일종의 nullptr 같은 개념으로 보자. 

	_int			m_iCurLevelID = -1; 
	CLevel*			m_pCurLevel = nullptr;
	CGameInstance*	m_pGameInstance = nullptr;

public:
	static CLevel_Manager* Create();
	virtual void Free() override;
};



END

