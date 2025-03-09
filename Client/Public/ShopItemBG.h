#pragma once
#include "UI.h"
#include "ShopPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CShopItemBG : public CShopPanel
{





protected:
	explicit CShopItemBG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CShopItemBG(const CShopItemBG& _Prototype);
	virtual ~CShopItemBG() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override {};
	virtual void			Late_Update(_float _fTimeDelta) override {};
	virtual void			Child_Update(_float _fTimeDelta) override;
	virtual void			Child_LateUpdate(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


public:
	void					Set_Pos(_float2 _vPos) { m_vShopBGPos = _vPos; }

private:
	void					isRender();
	
	

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CShopItemBG*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


protected:
	_uint					m_iTextureCount = { 0 };
	_uint					m_iSkillLevel = { 0 };

private:
	_float					m_fOpaque = { 0.0f };
	_float2					m_vShopBGPos = { 0.f, 0.f };
	_int					m_iPrice = { 0 };
	_wstring				m_strName = TEXT(" ");
	_wstring				m_strDialog = TEXT(" ");



	SKILLSHOP			m_eSkillShopIcon = SKILLSHOP_END;
	_bool				m_isChooseItem = { false };




};
END
