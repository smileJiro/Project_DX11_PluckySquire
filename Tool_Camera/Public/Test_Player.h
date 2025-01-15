#pragma once

#include "ContainerObject.h"

BEGIN(Camera_Tool)

class CTest_Player final : public CContainerObject
{
public:
	enum PART { PART_BODY, PART_LAST };

private:
	CTest_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTest_Player(const CTest_Player& _Prototype);
	virtual ~CTest_Player() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	void					Key_Input(_float _fTimeDelta);

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();

public:
	static CTest_Player*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END