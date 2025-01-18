#pragma once
#include "SettingPanel.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

class CESC_BackArrow : public CSettingPanel
{


protected:
	explicit CESC_BackArrow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CESC_BackArrow(const CESC_BackArrow& _Prototype);
	virtual ~CESC_BackArrow() = default;

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
	static CESC_BackArrow*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	//HRESULT					Cleanup_DeadReferences() override;



};

