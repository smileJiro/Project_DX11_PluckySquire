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
		CHAPTER2_BOOKMAGIC,

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


	HRESULT						Mapping_ExecuterTag();
	EVENT_EXECUTER_ACTION_TYPE	Find_ExecuterAction(const _wstring& _strTag);


public:
	HRESULT						Load_Trigger(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _szFilePath, CSection* _pSection = nullptr);
	HRESULT						Load_TriggerEvents(_wstring _szFilePath);

public:
	// Event Trigger
	void						On_End(_wstring _szEventTag);	// 끝나는 Action의 EventTag를 넘겨주고 현재 실행 중인 Action인지 확인(ex CutScene_1)

	void						Resister_TriggerEvent(_wstring _TriggerEventTag, _int _iTriggerID);


#pragma region Load 관련 함수 (COORDNATE 분기)
private :
	HRESULT						Fill_Trigger_3D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc);
	HRESULT						After_Initialize_Trigger_3D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc);

	HRESULT						Fill_Trigger_2D_Desc(json _TriggerJson, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc);
	HRESULT						After_Initialize_Trigger_2D(json _TriggerJson, CTriggerObject* _pTriggerObject, CTriggerObject::TRIGGEROBJECT_DESC& _tDesc, CSection* _pSection);
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
private:
	void						Resister_Event_Handler(_uint _iTriggerType, CTriggerObject* _pTrigger);
	void						Resister_Trigger_Action();

	_uint						Calculate_ExitDir(_fvector _vPos, _fvector _vOtherPos, PxBoxGeometry& _Box);

	void						Execute_Trigger_Event();

public:
	virtual void				Free() override;
};

END