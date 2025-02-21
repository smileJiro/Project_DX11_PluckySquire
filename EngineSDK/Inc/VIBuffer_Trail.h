#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
private:
	CVIBuffer_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& _Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint _iMaxVertexCount);
	virtual HRESULT Initialize(void* _pArg);

public:

private:
	_uint			m_iTriangleCount = 0;
	VTXPOSTEX*		m_pVertices = { nullptr }; // Prototype에서 공유하지 않음, 각 객체가 하나씩 가지도록 하자.

public:
	static CVIBuffer_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END