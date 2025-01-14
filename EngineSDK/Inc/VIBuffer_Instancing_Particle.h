#pragma once
#include "VIBuffer.h"

/* 파티클과 같은 동적으로 위치 변화가 큰 모델들을 다수 그려내기 위한 Buffer들의 부모 클래스 */
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Instancing_Particle abstract : public CVIBuffer
{
public:
	typedef struct tagParticleDesc
	{
		_uint			iNumInstances = 0;
		_float3			vCenter = {};
		_float3			vRange = {};			/* x, y, z */
		_float3			vPivot = {};

		_float2			vSpeed = {};			/* Min, Max */
		_float2			vSize = {};				/* Min, Max */
		_float2			vLifeTime = {};			/* Time, Acc */
		_bool			isLoop = false;

	}PARTICLE_DESC;
protected:
	CVIBuffer_Instancing_Particle(ID3D11Device* _pDeivce, ID3D11DeviceContext* _pContext);
	CVIBuffer_Instancing_Particle(const CVIBuffer_Instancing_Particle& _Prototype);
	virtual ~CVIBuffer_Instancing_Particle() = default;

public:
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* _pArg);
	virtual HRESULT				Render();
	virtual HRESULT				Bind_BufferDesc(); /* 기존 VIBuffer + InstancingBuffer Bind */

public: /* Paticle Effect */
	void Drop(_float _fTimeDelta); 
	void Spread(_float _fTimeDelta);
protected:
	ID3D11Buffer*				m_pVBInstance = nullptr;		/* Instancing Buffer */
	D3D11_BUFFER_DESC			m_InstanceBufferDesc = {};		/* Instancing Buffer도 VertexBuffer처럼 바인딩하기에 필요하다 */
	D3D11_SUBRESOURCE_DATA		m_InstanceInitialDesc = {};
	_uint						m_iNumInstances = {};			/* 인스턴스 개수 */
	_uint						m_iNumIndicesPerInstance = {};	/* 인스턴스 한개당 할당되는 인덱스의 개수 */
	_uint						m_iInstanceStride = {};			/* Instancing Buffer의 Stride */
	VTXINSTANCE*				m_pInstanceVertices = {};		/* 행렬정보를 담고있는 구조체 해당 데이터를 기반으로 Instancing Buffer를 생성할 것 임. */

protected: /* Particle Effect Data */
	_float3						m_vPivot = {};					/* 해당 피봇값을 기준으로 다양한 파티클 효과를 구현한다. */
	_bool						m_isLoop = false;				/* 해당 효과의 루프 여부 */
	_float*						m_pSpeeds = {};					/* 입자별 스피드 */
public:

	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END