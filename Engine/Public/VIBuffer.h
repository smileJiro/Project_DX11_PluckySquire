#pragma once
#include "Component.h"

/* 여러 메시(모델)을 그려내기 위한 컴포넌트의 부모클래스 */
/* 메시를 구성하기 위한 정점과 인덱스 버퍼를 저장 */
BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract: public CComponent
{
protected:
	CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer(const CVIBuffer& _Prototype);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual HRESULT Render();

public:
	virtual HRESULT Bind_BufferDesc();
public:
	// Get
	//const vector<_float3>& Get_VerticesPos() { return m_vecVerticesPos; };
	//const vector<_uint>& Get_IndexBuffer() { return m_vecIndexBuffer; }
	_uint Get_NumIndices() { return m_iNumIndices; }
	_uint Get_NumVertices() { return m_iNumVertices; }
	_uint Get_NumTriangles() { return m_iNumIndices / 3; }
protected:
	ID3D11Buffer* m_pVB = { nullptr };
	ID3D11Buffer* m_pIB = { nullptr };

protected:
	D3D11_BUFFER_DESC m_BufferDesc;
	D3D11_SUBRESOURCE_DATA m_SubResourceDesc;

protected:
	_uint m_iVertexStride = 0;
	_uint m_iNumVertices = 0;
	_uint m_iNumVertexBuffers = 0; // Dx11 에서는 여러개의 VertexBuffer를 InputAssembler에 전송할 수 있다. (이를 카운팅 하기 위한 변수) 

	_uint m_iIndexStride = 0;
	_uint m_iNumIndices = 0;
	DXGI_FORMAT m_eIndexFormat = {}; // Index 가 어떠한 Format을 가진 데이터 타입인지 장치에 알리는 용도.
	D3D11_PRIMITIVE_TOPOLOGY m_ePrimitiveTopology = {}; // 사용자가 의도한 Primitive의 형식을 장치에 알리는 용도.
	// Primitive 란 컴퓨터 공학에서 원초, 기본 즉 가장 작은 처리(Processing) 단위로 사용되는 경우가 많다.
	// 여기서의 Primitive도 렌더링에 필요한 최소 단위와 같은 의미로 사용된 것 같아 보인다.

	// 충돌을 위한 Vertex정보.
	vector<_float3> m_vecVerticesPos;
	vector<_float3> m_vecVerticesNormal;
	vector<_uint> m_vecIndexBuffer;
protected:
	/*해당 함수에 매개변수로 던진 Buffer에 값을 채워줌. (m_BufferDesc, m_SubResourceDesc) 의 데이터를 기반으로.*/
	HRESULT Create_Buffer(ID3D11Buffer** _ppOut);

public:
	virtual CComponent* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END