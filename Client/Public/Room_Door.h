#pragma once
#include "ContainerObject.h"

BEGIN(Client)

class CRoom_Door : public CContainerObject
{
public:
	enum DOOR_PART
	{
		DOOR_PART_DOOR = 0,
		DOOR_PART_KNOB = 1,
		DOOR_PART_DOORPLAQUE = 2,
		//DOOR_PART_STICKER_ZAP,	// 먼가 많음 합쳐져 있음
		DOOR_PART_STICKER_01,	// 소세지
		DOOR_PART_STICKER_02,	// 해골
		DOOR_PART_STICKER_04,	// 이상한 아저씨
		DOOR_PART_STICKER_05,	// 공룡
		DOOR_PART_STICKER_05_2,	// 공룡
		DOOR_PART_STICKER_07,	// Hey
		DOOR_PART_STICKER_09,	// 돼지
		DOOR_PART_STICKER_09_2,	// 돼지
		DOOR_PART_STICKER_09_3,	// 돼지
		DOOR_PART_STICKER_09_4,	// 돼지
		DOOR_PART_STICKER_10,	// 로켓
		DOOR_PART_STICKER_11,	// 할배
		//DOOR_PART_STICKER_12,	// 모르게씀
	
		DOOR_PART_END
	};

protected:
	CRoom_Door(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRoom_Door(const CRoom_Door& _Prototype);
	virtual ~CRoom_Door() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	_bool					Is_Turn_DoorKnob() { return m_isTurnDoorKnob; }

	void					Start_Turn_Door(_bool _isTurnDoor);
	void					Start_Turn_DoorKnob(_bool _isTurnDoor);

public:
	void					Turn_Door(_float _fTimeDelta);
	void					Turn_DoorKnob(_float _fTimeDelta);

private:
	_bool					m_isTurnDoorKnob = { false };
	_bool					m_isTurnDoor = { false };

	_float2					m_fTurnDoorTime = { 2.35f, 0.f };
	_float2					m_fTurnDoorKnobTime = { 0.7f, 0.f };

	_float					m_fTurnDoorAngle = { 110.f };
	_float					m_fTurnDoorKnobAngle = { 45.f };
	_float					m_fPreAngle = { 0.f };
	_float					m_fPreKnobAngle = { 0.f };

	_bool					m_isKnobDown = { false };

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

public:
	static CRoom_Door*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//virtual HRESULT			Cleanup_DeadReferences(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)
};

END