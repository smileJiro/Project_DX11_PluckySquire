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
	virtual void	Update(_float _fTimeDelta) override;

	virtual HRESULT Bind_BufferDesc();
	virtual HRESULT Render() override;

public:
	void	Add_Point(const _float4x4* _pWorldMatrix, _fvector _vPosition, _float _fLifeTime);
	//void	Delete_Point();
	void	Reset_Buffer() { m_iCurVertexCount = 0; }

public:
	_uint  Get_TrailCount() const { return m_iCurVertexCount; }

private:
	ID3D11ShaderResourceView* m_pBufferSRV = { nullptr };

private:
	_uint			m_iCurVertexCount = 0;
	


public:
	static CVIBuffer_Trail* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iMaxVertexCount);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END