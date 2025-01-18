#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance : public CVIBuffer
{
protected:
	CVIBuffer_Instance(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& _Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual HRESULT Render();

	virtual HRESULT Bind_BufferDesc();

public:
	_uint	Get_NumInstance() const { return m_iNumInstances; }

protected:
	ID3D11Buffer*				m_pVBInstance = { nullptr };		// Instance 전용 버퍼
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};			// Instance Buffer을 만들기 위한 Buffer Desc
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialDesc = {};			// Instance Buffer 초기 설정 Desc
	_uint						m_iNumInstances = {};				// Instance 개수
	_uint						m_iNumIndexCountPerInstance = {};	// Instance 한 개당 Index 할당 개수
	_uint						m_iInstanceStride = {};				// Instance Buffer 자료형의 크기


public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END