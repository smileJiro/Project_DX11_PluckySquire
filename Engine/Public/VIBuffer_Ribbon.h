#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Ribbon : public CVIBuffer
{
private:
	CVIBuffer_Ribbon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Ribbon(const CVIBuffer_Ribbon& _Prototype);
	virtual ~CVIBuffer_Ribbon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual HRESULT Bind_BufferDesc();
	virtual HRESULT Render();

private:
	VTXPOS*				m_pVertices = { nullptr };

public:
	static CVIBuffer_Ribbon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // 출발점, 끝점만 있는 Buffer
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END