#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CCandle_UI : public CModelObject
{
private:
	CCandle_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle_UI(const CCandle_UI& _Prototype);
	virtual ~CCandle_UI() = default;

public:

private: /* UI 처럼 렌더링을 해야해 직교투영으로 */
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix{}, m_ProjMatrix{};

	_uint					m_iDepth = {};
	_bool					m_bActive = { true };

public:
	static CCandle_UI* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle_UI* Clone(void* _pArg);
	virtual void Free();

};

END