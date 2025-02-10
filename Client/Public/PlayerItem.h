#pragma once
#include "TriggerObject.h"

BEGIN(Engine)
class CModelObject;
class CShader;
class C3DModel;
END

BEGIN(Client)
class CPlayerItem : public CTriggerObject
{
public:
	enum ITEM_MODE
	{
		DEFAULT,
		GETTING,
		DISAPPEAR,
		ITEM_MODE_END
	};

	typedef struct tagItemDesc : public CTriggerObject::TRIGGEROBJECT_DESC
	{
		_wstring		szModelTag = {};
	}PLAYERITEM_DESC;

private:
	CPlayerItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerItem(const CPlayerItem& _Prototype);
	virtual ~CPlayerItem() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Change_Mode(_uint _iItemMode) { m_iItemMode = _iItemMode; }

private:
	CShader*					m_pShaderCom = { nullptr };
	C3DModel*					m_pModelCom = { nullptr };

	//_uint						m_iRenderGroupID = {};
	//_uint						m_iPriorityID = {};

private:
	_uint						m_iItemType = {};
	_uint						m_iItemMode = {};

	_wstring					m_szModelTag = {};

private:
	HRESULT						Ready_Components(PLAYERITEM_DESC* _pArg);
	void						Action_Mode(_float _fTimeDelta);

	void						Action_Getting(_float _fTimeDelta);
	void						Action_Disappear(_float _fTimeDelta);

	HRESULT						Bind_ShaderResources_WVP();

public:
	static CTriggerObject*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};
END