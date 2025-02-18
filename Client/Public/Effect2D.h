#pragma once
#include "ModelObject.h"


BEGIN(Client)
class CEffect2D final : public CModelObject
{
public:
	typedef struct tagEffect2D : CModelObject::MODELOBJECT_DESC
	{
		
	}EFFECT2D_DESC;

private:
	CEffect2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect2D(const CEffect2D& _Prototype);
	virtual ~CEffect2D() = default;

public:
	virtual HRESULT			Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT			Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_float2					m_vDelayTime = {};
	_float2					m_vLifeTime = {};

private:
	_wstring				m_EffectPrototypeTag = {};
	
private:
	HRESULT					Ready_Components(EFFECT2D_DESC* _pDesc);

public:
	static CEffect2D*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;

};
END
