#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CDoor_2D : public CModelObject
{
public:
	enum DOOR_TYPE { RED, YELLOW, BLUE }; // RED : 몬스터로 열림, YELLOW : 다른 어떠한 트리거로 열림,  BLUE : 열쇠로 열림
	enum DOOR_SIZE { LARGE, MED, SMALL };
	enum DOOR_STATE { OPEN, CLOSE, OPENED, CLOSED };
	enum DOOR_ANIM {
		H_LARGE_RED_CLOSE = 0,
		H_LARGE_RED_CLOSED,
		H_LARGE_RED_OPEN,
		H_LARGE_RED_OPENED,

		V_LARGE_RED_CLOSE,
		V_LARGE_RED_CLOSED,
		V_LARGE_RED_OPEN,
		V_LARGE_RED_OPENED,

		H_MED_RED_CLOSE,
		H_MED_RED_CLOSED,
		H_MED_RED_OPEN = 10,
		H_MED_RED_OPENED,

		V_MED_RED_CLOSE,
		V_MED_RED_CLOSED,
		V_MED_RED_OPEN,
		V_MED_RED_OPENED,

		H_SMALL_RED_CLOSE,
		H_SMALL_RED_CLOSED,
		H_SMALL_RED_OPEN,
		H_SMALL_RED_OPENED,

		V_SMALL_RED_CLOSE = 20,
		V_SMALL_RED_CLOSED,
		V_SMALL_RED_OPEN,
		V_SMALL_RED_OPENED,

		H_LARGE_BLUE_CLOSED,
		H_LARGE_BLUE_OPEN,
		H_LARGE_BLUE_OPENED,

		H_MED_BLUE_CLOSED,
		H_MED_BLUE_OPEN,
		H_MED_BLUE_OPENED,

		H_SMALL_BLUE_CLOSED = 30,
		H_SMALL_BLUE_OPEN,
		H_SMALL_BLUE_OPENED,

		H_KEY_OPEN = 33,

		H_LARGE_YELLOW_CLOSE,
		H_LARGE_YELLOW_CLOSED,
		H_LARGE_YELLOW_OPEN,
		H_LARGE_YELLOW_OPENED,

		H_MED_YELLOW_CLOSE,
		H_MED_YELLOW_CLOSED,
		H_MED_YELLOW_OPEN = 40,
		H_MED_YELLOW_OPENED,

		H_SMALL_YELLOW_CLOSE,
		H_SMALL_YELLOW_CLOSED,
		H_SMALL_YELLOW_OPEN,
		H_SMALL_YELLOW_OPENED,

		V_SMALL_YELLOW_CLOSE,
		V_SMALL_YELLOW_CLOSED,
		V_SMALL_YELLOW_OPEN,
		V_SMALL_YELLOW_OPENED,
	};
public:
	typedef struct tagDoorDesc : public CModelObject::MODELOBJECT_DESC
	{
		_bool isHorizontal = true;
		DOOR_SIZE eSize = LARGE;
		DOOR_STATE eInitialState = CLOSED;
		_wstring  strSectionTag = L"";
	} DOOR_2D_DESC;

	

protected:
	CDoor_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDoor_2D(const CDoor_2D& _Prototype);
	virtual ~CDoor_2D() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

public:

protected:
	virtual void	Switch_Animation_By_State() = 0;

protected:
	_bool		m_isHorizontal = true;
	DOOR_TYPE	m_eDoorType = RED;
	DOOR_SIZE	m_eDoorSize = LARGE;
	DOOR_STATE	m_eDoorState = CLOSED;

public:
	virtual void			Free() override;
};

END