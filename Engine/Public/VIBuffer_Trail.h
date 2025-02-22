#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail : public CVIBuffer
{
public:
	typedef struct tagVIBufferTrailDesc
	{
		_float	fDisappearTime = { 0.019f };
	} TRAILBUFFER_DESC;

private:
	CVIBuffer_Trail(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& _Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint _iMaxVertexCount);
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Update(_float _fTimeDelta) override;

	virtual HRESULT Bind_BufferDesc();
	virtual HRESULT Render() override;

public:
	void	Set_DisappearTime(_float _fDisappearTime) { m_fDisappearTime = _fDisappearTime; }
	void	Update_Point(const _float4x4* _pWorldMatrix, _fvector _vPosition);
	void	Reset_Buffer() { m_iCurVertexCount = 0; }

public:
	_uint  Get_TrailCount() const { return m_iCurVertexCount; }

private:
	ID3D11ShaderResourceView* m_pBufferSRV = { nullptr };

private:
	_uint			m_iTriangleCount = 0;
	VTXTRAIL*		m_pVertices = { nullptr }; // Prototype에서 공유하지 않음, 각 객체가 하나씩 가지도록 하자.

	_float			m_fAccTime = 0.f;
	_float			m_fDisappearTime = 0.019f;
	_uint			m_iCurVertexCount = 0;
	
	_float			m_fOffsetTexcoord = 0.f;
	_float			m_fTexcoordPerIndex = 0.f;


public:
	static CVIBuffer_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END