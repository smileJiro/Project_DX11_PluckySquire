#pragma once
#include "3DMapObject.h"


BEGIN(Client)

class C3DMapSpskObject final : public C3DMapObject
{
private:
	C3DMapSpskObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapSpskObject(const C3DMapSpskObject& _Prototype);
	virtual ~C3DMapSpskObject() = default;

public :
	virtual HRESULT					Initialize(void* _pArg) override;

public :
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;
	virtual HRESULT					Render_WorldPosMap() override;


public :
	_wstring m_strRenderSectionTag = L"";

public:
	static C3DMapSpskObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
