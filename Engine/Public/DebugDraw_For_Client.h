#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CDebugDraw_For_Client final : public CComponent
{
private:
	CDebugDraw_For_Client(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDebugDraw_For_Client(const CDebugDraw_For_Client& _Prototype);
	virtual ~CDebugDraw_For_Client() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
#ifdef _DEBUG
public:
	HRESULT Render_Frustum(BoundingFrustum& _Frustum, _float4& _vDebugColor);
#endif

private:
#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = { nullptr };
	BasicEffect*						m_pEffect = { nullptr };
	ID3D11InputLayout*					m_pInputLayout = { nullptr };
	_float4								m_vDebugColor = { 0.0f, 1.0f, 0.0f, 1.0f };
#endif // _DEBUG


public:
	static CDebugDraw_For_Client* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CDebugDraw_For_Client* Clone(void* _pArg);
	virtual void Free() override;
};

END