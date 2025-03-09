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
		DOOR_PART_DOORPLATE = 2,

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
	//HRESULT					Add_PartObject(CPartObject* _pPartObject);

public:
//	void					Start_Turn_Knob();
//	void					Start_Turn_Door();
//
//private:
//	_bool					m_isTurnKnob = { false };
//	_bool					m_isTurnDoor = { false };

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