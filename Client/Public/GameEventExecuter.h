#pragma once

#include "GameObject.h"

/*
* 
* 
*	흐름
	
	0. Client\Bin\DataFiles\Trigger_Events.json
		새 json 객체 작성.
		"Actions"의 ActionTag를 "Create_EventExecuter" 로 맞춰야 한다.
		"EventTag" 에 실행할 태그를 이름지어서 순서대로 작성.

		ex. C02P0708_MonsterSpawnProcess 참조
		(   1. 번개가 내리친다
			2. 몬스터가 스폰된다)


	1. CTrigger_Manager.h :: enum EVENT_EXECUTER_ACTION_TYPE
		EventTag 종류별로 이름을 이름지어 추가하라.
	
	2. CTrigger_Manager.cpp ::Mapping_ExecuterTag()
		EVENT_EXECUTER_ACTION_TYPE와 EventTag를 매핑시켜라.
	
	3. switch_case에 해당하는 위치의 EVENT_EXECUTER_ACTION_TYPE에 하드코딩.
		자유롭게 써도 됨.


	해당 게임이벤트 모음을 트리거로 설정하고 싶다면,
	3D -> 김효림
	2D -> 박예슬
	문의

	250218_박예슬

*/



BEGIN(Client)

class CPlayer;
class CSampleBook;

class CGameEventExecuter abstract : public CGameObject
{
protected:
	enum STEP_TYPE
	{
		STEP_0,
		STEP_1,
		STEP_2,
		STEP_3,
		STEP_4,
		STEP_5,
		STEP_6,
		STEP_7,
		STEP_8,
		STEP_9,
		STEP_10,
		STEP_11,
		STEP_LAST
	};
public :
	typedef struct tagGameEventExecuter : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring	strEventTag;
	}EVENT_EXECUTER_DESC;
protected:
	CGameEventExecuter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CGameEventExecuter() = default;

public :
	virtual HRESULT					Initialize(void* _pArg);
	virtual void					Update(_float _fTimeDelta)  abstract;

	virtual void					GameEvent_End();

#pragma region 내부 구현할때 편하게 쓰라고 만듬

protected:
	_bool Is_Start()
	{
		if (!m_isStart)
		{
			m_isStart = true;
			return true;
		}
		return false;
	}
	_bool  Next_Step(_bool _isNextStep)
	{
		if (_isNextStep)
		{
			m_isStart = false;
			m_iStep++;
			m_iSubStep = 0;
			m_fTimer = 0.f;
		}
		return _isNextStep;
	}
	_bool  Next_Step_Over(_float _fMaxTime)
	{
		return Next_Step(m_fTimer > _fMaxTime);
	}
	
	_bool Step_Check(STEP_TYPE _eType) { return _eType == m_iStep; }
	
	CPlayer* Get_Player();
	CSampleBook* Get_Book();

#pragma endregion

protected:
	_float		m_fMaxTimer = 0.f;
	_float		m_fTimer = 0.f;
	_wstring	m_strEventTag;
	_wstring	m_strActionTag;
	_bool		m_IsSequence;


	// 메인 스텝(이벤트의 흐름)
	_uint		m_iStep = 0;
	//서브 스텝(스텝 내부의 흐름)
	_uint		m_iSubStep = 0;

	// 스텝을 처음 시작했는가?
	_bool		m_isStart = { false };
	
	//임시 데이터들 ...
	_bool			m_isPlag = { false };
	CGameObject*	m_pTargetObject = nullptr;
	_int		m_iEventExcuterAction = -1;

public:
	virtual void Free();

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;
};
END