#pragma once
#include "ModelObject.h"

BEGIN(Client)

class CLaser_Beam : public CModelObject
{
private:
	explicit CLaser_Beam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLaser_Beam(const CLaser_Beam& _Prototype);
	virtual ~CLaser_Beam() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	void					Update(_float _fTimeDelta) override;
	void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	
	void					Set_StartUV(_float2 _fStartUV) { m_fCutStartUV = _fStartUV; }
	void					Set_EndUV(_float2 _fEndUV) { m_fCutEndUV = _fEndUV; }

private :
	_float2 m_fCutStartUV = { 0.f, 1.f };
	_float2 m_fCutEndUV = { 1.f, 1.f };

public:
	static CModelObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END