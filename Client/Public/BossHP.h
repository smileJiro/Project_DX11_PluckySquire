#pragma once
#include "UI.h"
#include "BossHPBar.h"
#include "ButterGrump.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CBossHP final : public CUI
{
protected:
	explicit CBossHP(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBossHP(const CBossHP& _Prototype);
	virtual ~CBossHP() = default;

private:
	HRESULT					Create_BossHPBar();

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CBossHP*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


private:
	CBossHPBar*				m_pBossHPBar = { nullptr };
	CButterGrump*			m_pButterGtump = { nullptr };
	_bool					m_isNextphase = { false };
	_bool					m_isDeadPhase = { false };
	

};
END
