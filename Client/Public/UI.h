#pragma once
#include "UIObject.h"
#include "Client_Defines.h"
#include "ModelObject.h"
#include "UI_Manager.h"


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
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI(const CUI& Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_int _index = 0);


protected:
	virtual HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

protected:
	CShader* m_pShaderComs[COORDINATE_LAST] = {nullptr};
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_uint			m_iShaderPasses[COORDINATE_LAST] = {};
	_bool			m_isRender = { false };

};

END