#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

BEGIN(Camera_Tool)
class CBulbLine final : public CBase
{
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
	void				Set_Line(CGameObject* _pPoint);

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	CGameInstance*						m_pGameInstance = { nullptr };

private:
	pair<CGameObject*, CGameObject*>	m_Line;
	vector<class CBulb*>				m_Bulbs;

public:
	static CBulbLine*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CBulbLine*			Clone();
	virtual void		Free() override;
};
END
