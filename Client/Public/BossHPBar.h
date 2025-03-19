#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CBossHPBar final : public CUI
{

protected:
	explicit CBossHPBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBossHPBar(const CBossHPBar& _Prototype);
	virtual ~CBossHPBar() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	void					Set_BossCurHP(_uint _CurHP) { m_iBossCurHP = _CurHP; }
	void					Set_BossPhase();
	_uint					Get_BossPhase() { return m_iBossPhase; }


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CBossHPBar*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	HRESULT					Cal_BossHP();
	HRESULT					Change_Texture();

private:
	_bool					m_isMultextrue = { false };
	_uint					m_iTextureCount = { 0 };
	_int					m_iBossMaxHP = { 0 };
	_int					m_iBossCurHP = { 0 };
	_float2					m_vBossHPBarScale = { 0.f, 0.f };
	_float2					m_vOriginalPos = { 0.f, 0.f };
	_uint					m_iBossPhase = { 0 };
	_bool					m_isChangeTexture = { false };

};
END
