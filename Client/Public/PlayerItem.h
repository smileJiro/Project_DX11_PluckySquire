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
		GETTING,		// 먹고 위로 올라가기
		WAIT,
		DISAPPEAR,		// 없애기(다이얼로그 다 끝내면 없애면 됨)
		ITEM_MODE_END
	};

	typedef struct tagItemDesc : public CTriggerObject::TRIGGEROBJECT_DESC
	{
		_wstring		szModelTag = {};
		_wstring		szItemTag = {};
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
	void						Change_Mode(_uint _iItemMode);

private:
	CShader*					m_pShaderCom = { nullptr };
	C3DModel*					m_pModelCom = { nullptr };


private:
	_uint						m_iItemType = {};
	_uint						m_iItemMode = {};
	_float						m_fOriginY = {};
	_float						m_fOriginScale = { 4.f };

	_wstring					m_szModelTag = {};	// latch_glove
	_wstring					m_szItemTag = {};	// Flipping_Glove

	_float2						m_fScaleTime = { 0.6f, 0.f };
	_float2						m_fWaitTime = { 0.4f, 0.f };
	_bool						m_isScaleDown = { false };
	_bool						m_isFinishWait = { false };
	_bool						m_isStop = { false };
	_bool						m_isInFrustum = { false };

private:
	HRESULT						Ready_Components(PLAYERITEM_DESC* _pArg);
	void						Action_Mode(_float _fTimeDelta);

	void						Action_Getting(_float _fTimeDelta);
	void						Action_Wait(_float _fTimeDelta);
	void						Action_Disappear(_float _fTimeDelta);
	void						Check_PosY();

	HRESULT						Bind_ShaderResources_WVP();

public:
	static CPlayerItem*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};
END