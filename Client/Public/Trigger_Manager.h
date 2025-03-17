#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "TriggerObject.h"

/**
	박예슬 2025-02-06 추가

	**변경점 1.
	트리거 json 파일 로드시, 
	차원에 따라
	json 객체의 디스크립션 초기화 및
	실제 생성된 Trigger 객체의 외부 초기화 로직을 함수로 따로따로 분리함.

		Fill_Trigger_#D_Desc - 디스크립션 초기화
		After_Initialize_Trigger_#D - 객체 외부 초기화

	**변경점 2.
	Json 객체구조 중, 콜라이더 정보에 필요한 데이터는 전부 'Collider_Info'로 묶었음.
		CTrigger_Manager::Fill_Trigger_3D_Desc() 참조
	
	해당 변경점에 따라, Tool_Camera의 Json 파싱 코드도 전부 수정해 두었음
	(이것도 별로면 원 버전으로 바로 롤백 가능!)
*/

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTriggerObject;
END

BEGIN(Client)

class CSection;
class CTrigger_Manager final : public CBase
{
public :
	enum EVENT_EXECUTER_ACTION_TYPE
	{
		C02P0910_LIGHTNING_BOLT_SPAWN,
		C02P0910_MONSTER_SPAWN,
		START_TRAIN, // NPC 공용
		FRIEND_MAPENTER, // NPC 공용

		CHAPTER2_BOOKMAGIC,
		CHAPTER2_INTRO,
		CHAPTER2_HUMGRUMP,
		CHAPTER2_LUNCHBOX_APPEAR,	// 도시락에서 살려달라고 말함
		CHAPTER2_LUNCHBOX_OPEN,		// 도시락을 열고 나서의 책벌레
		CHAPTER2_BETTLE_PAGE,		// 비틀 조우 직전의 책벌레
		CHAPTER2_OPENBOOKEVENT,		// 2챕터 큰 책 앞에 도달한 책벌레
		CHAPTER2_STORYSEQUENCE,		// 글로브 습득 후, 스케치스페이스 이벤트
		CHAPTER2_AFTER_OPENING_BOOK,		// 3D 끝나고 책 열어서 책 속으로 들어갔을 때 나오는 이벤트
		CHAPTER2_GOING_TO_ARTIA,		// 3D 끝나고 책 열어서 책 속으로 들어갔을 때 나오는 이벤트
		CHAPTER2_FRIENDEVENT_0,		// 돌덩이 맵에서 처음 세명이 뭉친 타이밍
		CHAPTER2_FRIENDEVENT_1,		// 험그럼프 책의탑 가기전에 저트버리는 타이밍.
		CHAPTER2_PIP_0,		// 1챕터 찍찍이 처음 만나기

		CHAPTER4_3D_OUT_01,			// 3D로 나가는 맵 처음 들어갔을때 이벤트
		CHAPTER4_3D_OUT_02,			// 포탈쪽으로 갔을때의 이벤트
		CHAPTER4_INTRO,				// 김효림 카메라컷씬
		CHAPTER4_2D_INTRO,			// 처음 4챕터 진입했을 때
		CHAPTER4_INTRO_POSTIT_SEQUENCE, // 책벌레 처음 조우
		CHAPTER4_RIDE_ZIPLINE,		// Zipline 타기
		CHAPTER4_EVENT_FLAG,		// Zipline 들어가기 전 이벤트
		CHAPTER4_GATEEVENT,		// 성문 들어갓을떄 쓰러지는 이벤트
		CHAPTER4_STORYSEQUENCE,		// 글로브 습득 후, 스케치스페이스 이벤트
		//CHAPTER4_2D_IN,				// 폭탄 도장 얻고나서의 이벤트 

		CHAPTER6_INTRO,				// Intro
		CHAPTER6_START_3D,			// 톱니 앞에서 3D 진입 전

		/* Father Game Start */
		CHAPTER6_FATHERGAME_PROGRESS_START_CLEAR,
		CHAPTER6_FATHERGAME_PROGRESS_ZETPACK_CLEAR,
		CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_1,
		CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_2,
		CHAPTER6_FATHERGAME_PROGRESS_FATHERPART_3,
		/* Father Game End */
		CHAPTER6_HUMGRUMP_REVOLT,	// 달수염 죽는 이벤트
		CHAPTER6_CHANGE_BOOK_TO_GREATE_HUMGRUMP,		// 위대한 험그럼프로 책 바뀌는 이벤트
		CHAPTER6_STORYSEQUENCE_01, // 왕궁 이벤트

		/* Friend Event */
		CHAPTER6_FRIENDEVENT_0,
		/* Friend Event */

		CHAPTER8_LASER_STAGE,
		CHAPTER8_INTRO,
		CHAPTER8_MAP_INTRO,
		CHAPTER8_INTRO_POSTIT_SEQUENCE,
		CHAPTER8_OUTRO_POSTIT_SEQUENCE,
		CHAPTER8_SWORD,
		CHAPTER8_STOP_STAMP,
		CHAPTER8_BOMB_STAMP,
		CHAPTER8_TILTING_GLOVE,

		EVENT_EXECUTER_ACTION_TYPE_LAST
	};


	DECLARE_SINGLETON(CTrigger_Manager)

public:
	enum EXIT_RETURN_MASK
	{
		NONE = 0x00,
		RIGHT = 0x01,
		LEFT = 0x02,
		UP = 0x04,
		DOWN = 0x08,
		RETURN_MASK_END
	};

	typedef struct tagTriggerEvent
	{
		function<void(_wstring)>	Action;
		_wstring					EventTag;			// 진짜 사용할 EventTag, Trigger의 EventTag와 같음(ex CutScene_1)
		_bool						isSequence = {};	// True -> 해당 Action이 끝난 다음에 다음 Action이 실행한다, false -> 다음 Action도 바로 실행한다
		_bool						isOn = { false };
	} ACTION;

private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT						Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void						Update();

public:
	void						Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);

	HRESULT						Mapping_ExecuterTag();
	EVENT_EXECUTER_ACTION_TYPE	Find_ExecuterAction(const _wstring& _strTag);

public:
	HRESULT						Load_Trigger(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _szFilePath, CSection* _pSection = nullptr);
	HRESULT						Load_TriggerEvents(_wstring _szFilePath);

	HRESULT						Create_TriggerObject(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, void* _pArg, CSection* _pSection = nullptr, any _any = -1);
public:
	// Event Trigger
	void						On_End(_wstring _szEventTag);	// 끝나는 Action의 EventTag를 넘겨주고 현재 실행 중인 Action인지 확인(ex CutScene_1)
	void						On_End(_int _iEventExecuterActionType) { m_isRunningEvents[_iEventExecuterActionType] = false; }
	_bool						Is_EventExecuter_Running(_int _iEventExecuterActionType) { return m_isRunningEvents[_iEventExecuterActionType]; }

	_int						Get_Running_EventExecuterAction(); // 현재 진행 중인 EventExecuterActionType이 뭔지 리턴한다, 없다면  -1을 리턴함

	void						Register_TriggerEvent(_wstring _TriggerEventTag, _int _iTriggerID);


#pragma region Load 관련 함수 (COORDNATE 분기)
private :
	HRESULT						Fill_Trigger_3D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc);
	HRESULT						After_Initialize_Trigger_3D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc);
	HRESULT						After_Initialize_Trigger_3D(CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC* _pDesc, any _any = -1);

	HRESULT						Fill_Trigger_2D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc);
	HRESULT						After_Initialize_Trigger_2D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc, CSection* _pSection);
	HRESULT						After_Initialize_Trigger_2D(CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC* _pDesc, CSection* _pSection, any _any = -1);
#pragma endregion

	



private:
	CGameInstance*				m_pGameInstance = nullptr;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	// Event Trigger
	unordered_map<_wstring, queue<ACTION>>				m_TriggerEvents;
	unordered_map<_wstring, function<void(_wstring)>>	m_Actions;
	queue<ACTION>										m_CurTriggerEvent = {};
	vector<_wstring>									m_EventExecuterTags;

	_int												m_iTriggerID = {};
	_bool												m_isEventEnd = { false };

	_bool												m_isRunningEvents[EVENT_EXECUTER_ACTION_TYPE_LAST] = { false };

private:
	void						Register_Event_Handler(_uint _iTriggerType, CTriggerObject* _pTrigger, _uint _iCoordinateType);
	void						Register_Trigger_Action();

	_uint						Calculate_ExitDir(_fvector _vPos, _fvector _vOtherPos, PxBoxGeometry& _Box);
	_uint						Calculate_ExitDir(_float2 _vPos, _float2 _vOtherPos, _float2 _myExtents);

	void						Execute_Trigger_Event();

public:
	virtual void				Free() override;
};

END