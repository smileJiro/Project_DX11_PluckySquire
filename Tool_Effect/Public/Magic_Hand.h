#pragma once
#include "ContainerObject.h"

BEGIN(Engine)
class CEffect_System;
END

class CMagic_Hand : public CContainerObject
{
	enum MAGICHAND_PART
	{
		MAGICHAND_BODY = 0,
		MAGICHAND_EFFECT,
		MAGICHAND_END,
	};
private:
	CMagic_Hand(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMagic_Hand(const CMagic_Hand& _Prototype);
	virtual ~CMagic_Hand() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	class CMagic_Hand_Body* m_pMagicHandbody = { nullptr };
	class CEffect_System* m_pEffectSystem = { nullptr };

private:
	HRESULT					Ready_PartObjects();

public:
	static CMagic_Hand* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void					Free() override;
};
