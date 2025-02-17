#pragma once

#include "ContainerObject.h"

BEGIN(Client)

class CWord_Controller final : public CContainerObject
{
	enum PORTAL_PART
	{
		WORD_PART1 = 1,
		WORD_PART2 = 2,
		WORD_PART_LAST
	};
public:
	typedef struct tagWordControllerDesc : public CContainerObject::CONTAINEROBJ_DESC
	{

	}WORD_CONTROLLER_DESC;
private:
	CWord_Controller(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWord_Controller(const CWord_Controller& _Prototype);
	virtual ~CWord_Controller() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);

public :
	virtual HRESULT			Import(json _COntrollerJson);
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:


public:
	static CWord_Controller* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json _ControllerJson);
	CGameObject* Clone(void* _pArg) override;
	void					Free() override;
};

END