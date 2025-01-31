#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CPlayerSword :
    public CModelObject
{
public:
	typedef struct tagPlayerSwordDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float fThrowForce = 10.f;
		_float fHomingForce = 100.f;
		_float fRotationForce = 50.f;
		class CPlayer* pParent = nullptr;
	}PLAYER_SWORD_DESC;
private:
	CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerSword(const CPlayerSword& _Prototype);
	virtual ~CPlayerSword() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	void Throw(_fvector _vDirection);
	_bool Is_Flying() { return m_IsFlying; }
private:
	_float m_fThrowingPower = 15.f;
	_vector m_vThrowDirection = {};
	//¹ÛÀ¸·Î ³ª°¡·Á´Â Èû
	_float m_fOutingForce = 1.f;
	//²ø¾îµéÀÌ´Â Èû
	_float m_fCentripetalForce = 30.f;
	_float m_fRotationForce = 50.f;
	class CPlayer* m_pPlayer = nullptr;
	_bool m_IsFlying = false;
public:
	static CPlayerSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END