#pragma once
#include "VIBuffer.h"

/* 디버그용 렌더링을 수행하는 buffer_debug는 Linelist로 그리며, indexbuffer를 사용하지 않는다. */
/* 넉넉한 크기의 vb를 준비하고 동적으로 mapunmap을통해 vertex의 위치를 변경한다.*/
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_PxDebug final :  public CVIBuffer
{
private:
	CVIBuffer_PxDebug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_PxDebug(const CVIBuffer_PxDebug& _Prototype);
	virtual ~CVIBuffer_PxDebug() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVertices); /* 초기 사이즈를 넉넉하게 잡는다. */
	virtual HRESULT	Initialize(void* _pArg) override;
	virtual HRESULT Render() override;
	virtual void	Update_PxDebug(const PxRenderBuffer& _PxRenderBuffer);

public:
	HRESULT			Bind_BufferDesc() override;

private:
	_uint			m_iNumRenderLines = 0;
public:
	static CVIBuffer_PxDebug* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumLines);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};
END