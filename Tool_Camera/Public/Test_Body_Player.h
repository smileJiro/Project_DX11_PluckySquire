#pragma once
#include "ModelObject.h"

BEGIN(Camera_Tool)

class CTest_Body_Player final : public CModelObject
{
public:
	typedef struct tagTestBodyDesc : public CModelObject::MODELOBJECT_DESC
	{

	}TESTBODY_DESC;
private:
	CTest_Body_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTest_Body_Player(const CTest_Body_Player& _Prototype);
	virtual ~CTest_Body_Player() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() override;

private:
	HRESULT					Ready_Components();

public:
	static CTest_Body_Player* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END