#pragma once
#include "GameObject.h"

BEGIN(Map_Tool)

class C2DTrigger_Sample final : public CGameObject
{
public:

	typedef struct tag2DTrigger_SampleDesc : public CGameObject::GAMEOBJECT_DESC
	{
		int _tmp = 1;
	}TRIGGER_2D_DESC;

private:
	C2DTrigger_Sample(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C2DTrigger_Sample(const C2DTrigger_Sample& _Prototype);
	virtual ~C2DTrigger_Sample() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	_string					Get_TriggerKey() { return m_strTriggerKey; }
	void					Set_TriggerKey(_string _strTriggerKey) { m_strTriggerKey = _strTriggerKey; }

private:
	_float4x4				m_matWorld;
	_string					m_strTriggerKey;
private:
	class CCollider*		m_pColliderCom = nullptr;

public:
	static C2DTrigger_Sample*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*			Clone(void* _pArg) override;
	virtual void					Free() override;
	virtual HRESULT					Cleanup_DeadReferences() override; 
};
END
