#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CPlayer;
class CPlayerBody :
    public CModelObject
{
public:
	typedef struct tagPlayerBodyDesc : public CModelObject::MODELOBJECT_DESC
	{
		CPlayer* pPlayer = nullptr;
	}PLAYER_BODY_DESC;

private:
	explicit CPlayerBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPlayerBody(const CPlayerBody& _Prototype);
	virtual ~CPlayerBody() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
private:
	CPlayer* m_pPlayer = nullptr;
public:
	static CModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END