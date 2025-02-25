#pragma once
#include "3DMapObject.h"


BEGIN(Client)

class C3DMapSkspObject final : public C3DMapObject
{
public :
	enum SKSP_RENDER_TYPE
	{
		SKSP_DEFAULT,
		SKSP_FLAGS,
		SKSP_LAST
	};

private:
	C3DMapSkspObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C3DMapSkspObject(const C3DMapSkspObject& _Prototype);
	virtual ~C3DMapSkspObject() = default;

public :
	virtual HRESULT					Initialize(void* _pArg) override;

public :
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;
	virtual HRESULT					Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag) override;


private :
	HRESULT					Render_Default();
	HRESULT					Render_Flags();

public :
	_wstring			m_strRenderSectionTag = L"";
	SKSP_RENDER_TYPE	m_eSkspType = SKSP_DEFAULT;


public:
	static C3DMapSkspObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
