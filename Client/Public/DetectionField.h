#pragma once
#include "Component.h"

BEGIN(Engine)
class CDebugDraw_For_Client;
END

BEGIN(Client)

class CMonster;

class CDetectionField final : public CComponent
{
public:
	typedef struct tagDetectionFieldDesc
	{
		_float fRange;		//기본적으로 인식범위를 씀
		_float fFOVX;
		_float fFOVY;
		_float fOffset;
		CMonster* pOwner;
		CGameObject* pTarget;
		CDebugDraw_For_Client* pDraw;
	}DETECTIONFIELDDESC;

private:
	CDetectionField(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDetectionField(const CDetectionField& _Prototype);
	virtual ~CDetectionField() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Late_Update(_float _fTimeDelta);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

public:
	_bool IsTarget_In_Detection();

private:
	_float m_fRange = { 0.f };
	_float m_fFOVX = { 0.f };
	_float m_fFOVY = { 0.f };
	_float m_fOffset = { 0.f };
	CMonster* m_pOwner = { nullptr };
	CGameObject* m_pTarget = { nullptr };
	_bool		m_isColl = { false };

#ifdef _DEBUG
	CDebugDraw_For_Client*				m_pDraw = nullptr;
#endif // _DEBUG


public:
	static CDetectionField* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CDetectionField* Clone(void* _pArg);
	virtual void Free() override;
};

END