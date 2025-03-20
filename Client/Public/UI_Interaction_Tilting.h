#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CUI_Interaction_Tilting : public CUI
{


protected:
	explicit CUI_Interaction_Tilting(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CUI_Interaction_Tilting(const CUI_Interaction_Tilting& _Prototype);
	virtual ~CUI_Interaction_Tilting() = default;

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
	static CUI_Interaction_Tilting*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


private:
	_int					m_iTextureNum = { 0 };
	_bool					m_isBookInteract = { false };
};
END
