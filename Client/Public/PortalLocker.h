#pragma once
#include "ModelObject.h"
/* 1. Portal Locker는 자기자신이속한 섹션의 Portal을 Active False 시킨다. */
/* 2. Portal Lock을 풀 수 있는 방법은 Portal Locker의 삭제 뿐이다. */

BEGIN(Client)
class CPortal;
class CPortalLocker final : public CModelObject
{
public:
	enum PORTALLOCKER_TYPE { TYPE_PURPLE = 0, TYPE_YELLOW = 2, TYPE_LAST };
	enum STATE { STATE_LOCK, STATE_OPEN, STATE_DEAD, STATE_LAST };
	typedef struct tagPortalLockerDesc : CModelObject::MODELOBJECT_DESC
	{
		CPortal* pTargetPortal = nullptr;
		CPortalLocker::PORTALLOCKER_TYPE ePortalLockerType = TYPE_PURPLE;
		_wstring strSectionKey;
	}PORTALLOCKER_DESC;

protected:
	CPortalLocker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortalLocker(const CPortalLocker& _Prototype);
	virtual ~CPortalLocker() = default;

public:
	virtual HRESULT				Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT				Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Open_Locker() { m_eCurState = STATE_OPEN; State_Change(); }

private:
	CPortal*					m_pTargetPortal = nullptr;

private:
	PORTALLOCKER_TYPE			m_ePortalLockerType = PORTALLOCKER_TYPE::TYPE_PURPLE;
	STATE						m_ePreState = STATE::STATE_LAST;
	STATE						m_eCurState = STATE::STATE_LAST;

private:
	void						State_Change();
	void						State_Change_Lock();
	void						State_Change_Open();
	void						State_Change_Dead();

private:
	void						Action_State(_float _fTimeDelta);
	void						Action_State_Lock(_float _fTimeDelta);
	void						Action_State_Open(_float _fTimeDelta);
	void						Action_State_Dead(_float _fTimeDelta);
	
private:
	void						On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);
public:
	static CPortalLocker*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortalLocker*				Clone(void* _pArg);
	void						Free() override;
};

END