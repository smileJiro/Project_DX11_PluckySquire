#pragma once
#include "VIBuffer.h"

/* 디버그용 렌더링을 수행하는 buffer_debug는 Linelist로 그리며, indexbuffer를 사용하지 않는다. */
/* 넉넉한 크기의 vb를 준비하고 동적으로 mapunmap을통해 vertex의 위치를 변경한다.*/
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Debug final :  public CVIBuffer
{
private:
	CVIBuffer_Debug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Debug(const CVIBuffer_Debug& _Prototype);
	virtual ~CVIBuffer_Debug() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumVertices); /* 초기 사이즈를 넉넉하게 잡는다. */
	virtual HRESULT	Initialize(void* _pArg);

public:
	static CVIBuffer_Debug* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};
END