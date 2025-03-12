#pragma once
#include "GameObject.h"

BEGIN(Map_Tool)

class C2DTrigger_Sample final : public CGameObject
{
public :
	enum TIRGGER_COLOR_MODE
	{
		DEFAULT_COLOR,
		PICKING_COLOR,
		LAST
	};

public:

	typedef struct tag2DTrigger_SampleDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 fDebugColor = { 0.0f, 1.0f, 0.0f, 1.0f };
		_float2 fRenderTargetSize = { };
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

	void					Change_Color(TIRGGER_COLOR_MODE _eMode);

	_string					Get_TriggerKey() { return m_strTriggerKey; }
	void					Set_TriggerKey(_string _strTriggerKey) { m_strTriggerKey = _strTriggerKey; }



	_bool					Is_Floor() { return m_isFloor; }
	void					Set_Floor(_bool _isFloor) { m_isFloor = _isFloor; }

	_bool					Is_JumpPass() { return m_isJumpPass; }
	void					Set_JumpPass(_bool _isJumpPass) { m_isJumpPass = _isJumpPass; }
private:
	_float4x4				m_matWorld;
	_string					m_strTriggerKey;
	_float2					m_fRenderTargetSize = {};
	_bool					m_isFloor = false;
	_bool					m_isJumpPass = false;
private:
	class CCollider*		m_pColliderCom = nullptr;

public:
	static C2DTrigger_Sample*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*			Clone(void* _pArg) override;
	virtual void					Free() override;
	virtual HRESULT					Cleanup_DeadReferences() override; 
};
END
