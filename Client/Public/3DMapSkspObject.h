#pragma once
#include "3DMapObject.h"


BEGIN(Client)

class C3DMapSkspObject final : public C3DMapObject
{
public :

public :
	enum SKSP_RENDER_TYPE
	{
		SKSP_NONE,
		SKSP_DEFAULT,
		SKSP_CUP,
		SKSP_TUB,
		SKSP_PLAG,
		SKSP_STORAGE,
		SKSP_POSTIT,
		SKSP_C09_ROTATE,
		SKSP_C09_TUBE,
		SKSP_LAST

	};

	enum SKSP_RENDER_PLAG
	{
		RT_RENDER_DEAFULT 
		,RT_RENDER_ROTATE 
		,RT_RENDER_UVRENDER 
		,RT_RENDER_FRAC 
		,RT_RENDER_UVRENDER_FRAC
		,RT_RENDER_LAST
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
	virtual HRESULT					Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag) override;
	_wstring						Get_RenderSectionTag() { return m_strRenderSectionTag; }
	virtual void					Register_WorldCapture();
private :
	void					Change_RenderState(RT_RENDERSTATE _eRenderState, _bool _isMapped);


	HRESULT					Render_Default();
	HRESULT					Render_Flags();
	HRESULT					Render_Cup();
	HRESULT					Render_Tub();
	HRESULT					Render_Storage();
	HRESULT					Render_Rotate();
	HRESULT					Render_Tube();

private :
	_wstring			m_strRenderSectionTag = L"";
	SKSP_RENDER_TYPE	m_eSkspType = SKSP_DEFAULT;
	RT_RENDERSTATE		m_eCurRenderState = RT_RENDERSTATE::RENDERSTATE_LIGHT;

	_bool				m_isRight = false;

public:
	static C3DMapSkspObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};
END
