#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CExcavator_Switch final : public CModelObject
{
	enum STATE { STATE_LOCK, STATE_OPEN, STATE_LAST };
	typedef struct tagSwitchDesc : public CModelObject::MODELOBJECT_DESC
	{

	}SWITCH_DESC;
public:
	CExcavator_Switch(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CExcavator_Switch(const CExcavator_Switch& _Prototype);
	virtual ~CExcavator_Switch() = default;

public:
	virtual HRESULT				Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

private:
	void						State_Change();
	void						State_Change_Lock();
	void						State_Change_Open();

private:
	void						Action_State(_float _fTimeDelta);
	void						Action_State_Lock(_float _fTimeDelta);
	void						Action_State_Open(_float _fTimeDelta);

private:
	HRESULT						Ready_Components(SWITCH_DESC* _pDesc);

public:
	static CExcavator_Switch*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*				Clone(void* _pArg);
	void						Free() override;
};

END