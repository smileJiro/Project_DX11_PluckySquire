#pragma once
#include "PartObject.h"

BEGIN(Engine)
class CEffect_System;
END
BEGIN(Client)
class CEffect_Beam;

class CTurnBookEffect : public CPartObject
{
public:


public:
	typedef struct tagTurnBookEffect : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pBodyMatrix = { nullptr };
		const _float4x4* pLHandMatrix = { nullptr };
		const _float4x4* pRHandMatrix = { nullptr };
	} TURNBOOKEFFECT_DESC;

private:
	CTurnBookEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTurnBookEffect(const CTurnBookEffect& _Prototype);
	virtual ~CTurnBookEffect() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;

public:
	void	Set_BookPosition(_fvector _vPosition);

public:
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

private:
	class CEffect_Beam* m_pLHandBeam = { nullptr };
	class CEffect_Beam* m_pRHandBeam = { nullptr };
	class CEffect_System* m_pLHandRing = { nullptr };
	class CEffect_System* m_pRHandRing = { nullptr };

public:
	static CTurnBookEffect* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END