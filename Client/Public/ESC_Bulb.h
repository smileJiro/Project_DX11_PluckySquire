#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class ESC_Bulb : public CSettingPanel
{


protected:
	explicit ESC_Bulb(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit ESC_Bulb(const ESC_Bulb& _Prototype);
	virtual ~ESC_Bulb() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_int					m_iPlayerBulbCount = { 0 };

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static ESC_Bulb*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//HRESULT					Cleanup_DeadReferences() override;



};

