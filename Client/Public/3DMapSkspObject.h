#pragma once
#include "3DMapObject.h"


BEGIN(Client)

class C3DMapSkspObject final : public C3DMapObject
{
private:
	C3DMapSkspObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapSkspObject(const C3DMapSkspObject& _Prototype);
	virtual ~C3DMapSkspObject() = default;

public :
	virtual HRESULT					Initialize(void* _pArg) override;

public :
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag) override;


public :
	_wstring m_strRenderSectionTag = L"";

public:
	static C3DMapSkspObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
