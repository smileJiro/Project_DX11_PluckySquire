#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CBombStamp final : public CUI
{

protected:
	explicit CBombStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBombStamp(const CBombStamp& _Prototype);
	virtual ~CBombStamp() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


private:
	CUI_Manager::STAMP		m_ePreStamp = { CUI_Manager::STAMP_END };
	_bool					m_isSmall	= {false};
	_bool					m_isBig = { false };

private:
	void					ChangeStamp(_float _fTimeDelta);

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CBombStamp*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

};

