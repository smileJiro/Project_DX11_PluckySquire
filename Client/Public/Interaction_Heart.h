#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CInteraction_Heart : public CUI
{


protected:
	explicit CInteraction_Heart(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CInteraction_Heart(const CInteraction_Heart& _Prototype);
	virtual ~CInteraction_Heart() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_int					m_PlayerHP = { 0 };
	_int					m_PrePlayerHP = { 0 };
	_vector					m_vPlayerPos = {};
	_float					m_fRenderTime = { 0.f };

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CInteraction_Heart*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	void					Cal_HeartPos();
	void					Cal_RenderTime(_float _fTimeDelta, _int _iPlayerHP);

};

