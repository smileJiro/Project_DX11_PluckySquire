#pragma once
#include "ModelObject.h"
/* 1. 단순 렌더 및 애니메이션 재생 등을 맡는 클라이언트 Body 클래스*/
BEGIN(Client)
class CFriendBody : public CModelObject
{
protected:
	CFriendBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFriendBody(const CFriendBody& _Prototype);
	virtual ~CFriendBody() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override; 
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;

public:
	static CFriendBody* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free() override;
};

END