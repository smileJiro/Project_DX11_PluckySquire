#pragma once

#include "GameObject.h"
#include "Postit_Page.h"

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
class CBook;

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
		STEP_12,
		STEP_13,
		STEP_14,
		STEP_15,
		STEP_LAST
	};
public :
	typedef struct tagGameEventExecuter : public CGameObject::GAMEOBJECT_DESC
	{
		_wstring	strEventTag;
		_bool		isChapterChangeEvent = false;
	}EVENT_EXECUTER_DESC;
protected:
	CGameEventExecuter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CGameEventExecuter() = default;

public :
	virtual HRESULT					Initialize(void* _pArg);
	virtual void					Priority_Update(_float _fTimeDelta);
	virtual void					Update(_float _fTimeDelta)  abstract;

	virtual void					GameEvent_End();

#pragma region 내부 구현할때 편하게 쓰라고 만듬

protected:
	_bool Is_Start() //  1 회 호출 
	{
		if (!m_isStart)
		{
			m_isStart = true;
			return true;
		}
		return false;
	}
	_bool  Next_Step(_bool _isNextStep) // 곧바로 다음 스텝으로 이동
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
	_bool  Next_Step_Over(_float _fMaxTime) // x 초 후 다음 스텝으로 이동
	{
		return Next_Step(m_fTimer > _fMaxTime);
	}
	
	_bool Step_Check(STEP_TYPE _eType) { return _eType == m_iStep; } // 현재 스텝 검사
	
	_bool Postit_Process(const _wstring& _strPostItSectionTag,  // 포스트잇이 어느 섹션에 있니?
									const _wstring& _strPostItDialogTag,  // 실행할 다이얼로그는 무엇이니?
									_float _fStartChaseCameraTime = 1.f, // 몇초동안 포스트잇을 타게팅하러 움직일거니?
									CPostit_Page::POSTIT_PAGE_POSTION_TYPE _ePostionType							//
										= CPostit_Page::POSTIT_PAGE_POSTION_TYPE::POSTIT_PAGE_POSTION_TYPE_A,	// 책벌레가 섹션 내부 어디에 호출될거니?
									_bool _isResetStart = true, // 포스트잇 실행 전에 특수한 카메라 움직임이 있었니?
									function<void()> _FirstCamFunc = nullptr); // 카메라가 타게팅 뒤 특수한 카메라 처리가 필요하니?


	_bool Postit_Process_Start(const _wstring& _strPostItSectionTag,	
		_float _fStartChaseCameraTime = 1.f,
		_bool _isResetStart = true,											
		function<void()> _FirstCamFunc = nullptr);							

	_bool Postit_Process_PageAppear(
		CPostit_Page::POSTIT_PAGE_POSTION_TYPE _ePostionType
		= CPostit_Page::POSTIT_PAGE_POSTION_TYPE::POSTIT_PAGE_POSTION_TYPE_A);
	
	_bool Postit_Process_PageTalk(const _wstring& _strPostItDialogTag,
		CPostit_Page::POSTIT_PAGE_POSTION_TYPE _ePostionType
		= CPostit_Page::POSTIT_PAGE_POSTION_TYPE::POSTIT_PAGE_POSTION_TYPE_A
	);

	_bool Postit_Process_End(_float _fStartChaseCameraTime);



	CGameObject* Find_Postit();
	CPlayer*	Get_Player();
	CBook*		Get_Book();

	_bool Setting_Postit_Page(const _wstring& _strPostItSectionTag);

	_bool Next_Event_Process(_float _fTimeDelta);

#pragma endregion

protected:
	_bool		m_isNextChapter = false;
	_wstring	m_strNextChapter = L"";
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
	CGameObject*	m_pTargetObject_Second = nullptr;
	CPostit_Page*	m_pPostitPage = nullptr;
	_int			m_iEventExcuterAction = -1;

public:
	virtual void Free();

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT Cleanup_DeadReferences() override;
};
END