#pragma once
#include "VIBuffer.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Vertical final : public CVIBuffer
{
private:
	CVIBuffer_Rect_Vertical(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Rect_Vertical(const CVIBuffer_Rect_Vertical& _Prototype);
	virtual ~CVIBuffer_Rect_Vertical() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);

public:
	
public:
	static CVIBuffer_Rect_Vertical* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END