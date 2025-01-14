#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI : public Engine::CUIObject
{
public:
	typedef struct : public CUIObject::UIOBJECT_DESC
	{

	}UIOBJDESC;


public:
	explicit CUI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CUI(const CUI& _Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void Priority_Update(_float _fTimeDelta);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update(_float _fTimeDelta);
	virtual HRESULT Render();


protected:
	virtual HRESULT Ready_Components() = 0;
	HRESULT Bind_ShaderResources();


public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;

protected:
	CTexture*			m_pTextureCom	= { nullptr };
	CVIBuffer_Rect*		m_pVIBufferCom	= { nullptr };

	CShader*				m_pShaderComs[COORDINATE_LAST] = {};
	_uint					m_iShaderPasses[COORDINATE_LAST] = {};

};

END