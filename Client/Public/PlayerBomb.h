#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CPlayerBomb :
    public CModelObject
{
public:
	enum ANIMATION_STATE
	{
		EXPLODE = 0,
		IDLE,
	};
private:
	explicit CPlayerBomb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPlayerBomb(const CPlayerBomb& _Prototype);
	virtual ~CPlayerBomb() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:

public:
	static CPlayerBomb* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END