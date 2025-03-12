#pragma once
#include "PortalLocker.h"
BEGIN(Engine)
class CLayer;
END
BEGIN(Client)
class CPortalLocker_LayerCount final: public CPortalLocker
{
public:
	typedef struct tagPortalLockerLayerCountDesc : CPortalLocker::PORTALLOCKER_DESC
	{
		_wstring strCountingLayerTag;
	}PORTALLOCKER_LAYER_DESC;
private:
	CPortalLocker_LayerCount(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortalLocker_LayerCount(const CPortalLocker_LayerCount& _Prototype);
	virtual ~CPortalLocker_LayerCount() = default;


public:
	virtual HRESULT						Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT						Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.

private:
	virtual void						Action_State_Lock(_float _fTimeDelta);

private:
	_wstring							m_strCountingLayerTag;
	// CLayer*								m_pCountingLayer = nullptr; // Layer는 객체 수명관리에 어려움이 있다. 직접 참조는 x

private:
	_int								m_iAliveObjectCount = 0;

private:
	_int								m_iMaxCheckCount = 3;			// 총 3회(3프레임)의 검사를 수행한다.
	_int								m_iCheckCount = 0;

public:
	static CPortalLocker_LayerCount*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*						Clone(void* _pArg) override;
	void								Free() override;

};

END