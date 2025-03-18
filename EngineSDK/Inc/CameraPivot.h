#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCameraPivot final : public CGameObject 
{
public:
	typedef struct tagCameraPivotDesc : public CGameObject::GAMEOBJECT_DESC
	{
		CGameObject*		pMainTaget = { nullptr };
		CGameObject*		pSubTarget = { nullptr };
		_float				fRatio = {};
	}CAMERAPIVOT_DESC;

protected:
	CCameraPivot(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCameraPivot(const CCameraPivot& _Prototype);
	virtual ~CCameraPivot() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					Set_MainTarget(CGameObject* _pMainTarget); 
	void					Set_SubTarget(CGameObject* _pSubTarget);
	void					Set_Ratio(_float _fRatio) { m_fRatio = _fRatio; }

	CGameObject*			Get_MainTarget() { return m_pMainTarget; }
private:
	CGameObject*			m_pMainTarget = { nullptr };
	CGameObject*			m_pSubTarget = { nullptr };

	_float					m_fRatio = {};

private:
	void					Calculate_FinalPosition();

public:
	static CCameraPivot*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	virtual HRESULT			Cleanup_DeadReferences(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)
};

END