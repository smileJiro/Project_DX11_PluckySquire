#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class CVIBuffer_Cell final : public CVIBuffer
{
	CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& _Prototype);
	virtual ~CVIBuffer_Cell() = default;

public:
	/* 객체 원형 생성 시, 자기 자신을 구성하는 정점 세개를 받아 저장한다. */
	virtual HRESULT Initialize_Prototype(const _float3* _pPoints);
	virtual HRESULT Initialize(void* _pArg) override;

public:
	static CVIBuffer_Cell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};
END
