#pragma once
#include "TriggerObject.h"

BEGIN(Camera_Tool)

class CCamera_Trigger final : public CTriggerObject
{
public:
	enum CAMERA_TRIGGER_TYPE 
	{
		ARM_TRIGGER,
		CUTSCENE_TRIGGER,
		FREEZE_X,
		FREEZE_Z,
		CAMERA_TRIGGER_TYPE_END
	};

public:
	typedef struct tagCamerTriggerDesc : public CTriggerObject::TRIGGEROBJECT_DESC
	{
		_uint			iCameraTriggerType = {};
		_wstring		szEventTag = {};

		_bool			isReturn = { false };
	}CAMERA_TRIGGER_DESC;

private:
	CCamera_Trigger(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Trigger(const CCamera_Trigger& _Prototype);
	virtual ~CCamera_Trigger() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

public:
	_uint						Get_CameraTriggerType() { return m_iCameraTriggerType; }
	_wstring					Get_CameraTriggerEventTag() { return m_szEventTag; }

	void						Set_CameraTriggerType(_uint _iCameraTriggerType) { m_iCameraTriggerType = _iCameraTriggerType; }
	void						Set_CameraTriggerEventTag(_wstring _wsEventTag) { m_szEventTag = _wsEventTag; }

public:
	virtual void				OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void				OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void				OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other) override;

private:
	_uint						m_iCameraTriggerType = { CAMERA_TRIGGER_TYPE_END };
	_wstring					m_szEventTag = {};			// CutScene이나 Arm 이름 같은 거

	_bool						m_isReturn = { false };		// Exit 후 Defautl Arm 벡터로 돌아갈 건지 말 건지

public:
	static CCamera_Trigger*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};

END