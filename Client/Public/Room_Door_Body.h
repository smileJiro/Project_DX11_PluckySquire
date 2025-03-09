#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CRoom_Door;

class CRoom_Door_Body : public CModelObject
{
public:
	typedef struct tagRoomDoorBodyDesc : public CModelObject::MODELOBJECT_DESC
	{
		CRoom_Door*		pRoomDoor = nullptr;
	}ROOM_DOOR_BODY_DESC;

private:
	explicit CRoom_Door_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CRoom_Door_Body(const CRoom_Door_Body& _Prototype);
	virtual ~CRoom_Door_Body() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	void					Update(_float _fTimeDelta) override;
	void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CRoom_Door*				m_pRoomDoor = { nullptr };

public:
	static CModelObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END