#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CESC_Enter : public CSettingPanel
{


protected:
	explicit CESC_Enter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CESC_Enter(const CESC_Enter& _Prototype);
	virtual ~CESC_Enter() = default;

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
	static CESC_Enter*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//HRESULT					Cleanup_DeadReferences() override;



};

