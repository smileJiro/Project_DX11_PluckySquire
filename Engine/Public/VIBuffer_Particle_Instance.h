#pragma once
#include "VIBuffer.h"

/* 모든 파티클(포인트, 스프라이트, 모델)들의 부모입니다 */

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Instance abstract : public CVIBuffer
{

protected:
	CVIBuffer_Particle_Instance(ID3D11Device* _pDeivce, ID3D11DeviceContext* _pContext);
	CVIBuffer_Particle_Instance(const CVIBuffer_Particle_Instance& _Prototype);
	virtual ~CVIBuffer_Particle_Instance() = default;

public:
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* _pArg);
	virtual HRESULT				Render();
	virtual HRESULT				Bind_BufferDesc(); /* 기존 VIBuffer + InstancingBuffer Bind */

protected:
	ID3D11Buffer*				m_pVBInstance = nullptr;		/* Instancing Buffer */
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};		/* Instancing Buffer도 VertexBuffer처럼 바인딩하기에 필요하다 */
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialDesc = {};
	_uint						m_iNumInstances = {};			/* 인스턴스 개수 */
	_uint						m_iNumIndicesPerInstance = {};	/* 인스턴스 한개당 할당되는 인덱스의 개수 */
	_uint						m_iInstanceStride = {};			/* Instancing Buffer의 Stride */

public:

	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END