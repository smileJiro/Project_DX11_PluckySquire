#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_NonUV final : public CVIBuffer
{
private:
	CVIBuffer_Rect_NonUV(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Rect_NonUV(const CVIBuffer_Rect_NonUV& _Prototype);
	virtual ~CVIBuffer_Rect_NonUV() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);

public:
	
public:
	static CVIBuffer_Rect_NonUV* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END