#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Camera_Tool)
class CBulbLine final : public CBase
{
public:
	typedef struct tagBulbLineDesc //: //public CModelObject::MODELOBJECT_DESC
	{
		_float fBulbPosOffset = {};
	}BULBLINE_DESC;

private:
	CBulbLine(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBulbLine(const CBulbLine& _Prototype);
	virtual ~CBulbLine() = default;

public:
	HRESULT				Initialize(void* pArg);
	HRESULT				Initialize_Clone();
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);

public:
	pair<CModelObject*, CModelObject*>* Get_Line() { return &m_Line; }
	_uint				Get_BulbCount() { return m_Bulbs.size(); }
	list<class CBulb*>* Get_Bulbs() { return &m_Bulbs; }
	
	void				Set_BulbPosOffset(_float _fBulbPosOffset) { m_fBulbPosOffset = _fBulbPosOffset; }

public:
	void				Add_Point(CModelObject* _pPoint);
	void				Edit_BulbInfo();
	void				Clear();

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CGameInstance*						m_pGameInstance = { nullptr };

private:
	pair<CModelObject*, CModelObject*>	m_Line;
	list<class CBulb*>					m_Bulbs;

	_float								m_fBulbPosOffset = {};

private:
	HRESULT				Create_Bulbs();
	//HRESULT				Create_Bulb();

public:
	static CBulbLine*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CBulbLine*			Clone();
	virtual void		Free() override;
};
END
