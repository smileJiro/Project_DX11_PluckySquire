#pragma once
#include "Component.h"

BEGIN(Engine)
class CDebugDraw_For_Client;
END

BEGIN(Client)

class CMonster;

class CSneak_DetectionField final : public CComponent
{
public:
	typedef struct tagSneakDetectionFieldDesc
	{
		_float fRadius;		//¿ø ¹üÀ§
		_float fOffset = 0.f;
		CMonster* pOwner;
		CGameObject* pTarget;
#ifdef _DEBUG
		CDebugDraw_For_Client* pDraw;
#endif // _DEBUG
	}SNEAKDETECTIONFIELDDESC;

private:
	CSneak_DetectionField(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_DetectionField(const CSneak_DetectionField& _Prototype);
	virtual ~CSneak_DetectionField() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Late_Update(_float _fTimeDelta);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

public:
	_bool IsTarget_In_SneakDetection();

private:
	_float m_fRadius = { 0.f };
	_float m_fOffset = { 0.f };
	CMonster* m_pOwner = { nullptr };
	CGameObject* m_pTarget = { nullptr };
	_bool		m_isColl = { false };

#ifdef _DEBUG
	CDebugDraw_For_Client*				m_pDraw = nullptr;
#endif // _DEBUG


public:
	static CSneak_DetectionField* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CSneak_DetectionField* Clone(void* _pArg);
	virtual void Free() override;
};

END