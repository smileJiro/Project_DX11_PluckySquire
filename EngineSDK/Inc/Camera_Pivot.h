#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera_Pivot final : public CGameObject 
{
public:
	typedef struct tagCameraPivotDesc : public CGameObject::GAMEOBJECT_DESC
	{
		CGameObject*		pMainTaget = { nullptr };
		CGameObject*		pSubTarget = { nullptr };
		_float				fRatio = {};
	}CAMERAPIVOT_DESC;

protected:
	CCamera_Pivot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Pivot(const CCamera_Pivot& _Prototype);
	virtual ~CCamera_Pivot() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	CGameObject*			m_pMainTarget = { nullptr };
	CGameObject*			m_pSubTarget = { nullptr };

	_float					m_fRatio = {};

private:
	void					Calculate_FinalPosition();

public:
	static CCamera_Pivot*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	virtual HRESULT			Cleanup_DeadReferences(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)
};

END