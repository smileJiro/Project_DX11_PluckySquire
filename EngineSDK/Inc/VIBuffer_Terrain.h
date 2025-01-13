#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
public:
	typedef struct 
	{
		_tchar* pHeightMapFilePath; // 높이 맵의 픽셀 수 만큼 버텍스 찍기.
		_float fVerticesX, fHeightY, fVerticesZ;
		_float fVerticesItv; // 버텍스 간 간격 정도 지정 할 수 있겠지.
	}VIBUFFER_TERRAIN_DESC;

private:
	CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& _Prototype);
	virtual ~CVIBuffer_Terrain() = default;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
public:
	virtual HRESULT Remake_Buffer_Height(void* _pArg);
	virtual HRESULT Remake_Buffer_Flat(void* _pArg);

public:
	_uint Get_VerticesX(){ return m_iNumVerticesX; }
	_uint Get_VerticesZ(){ return m_iNumVerticesZ; }

private:
	_uint			m_iNumVerticesX = { 0 };
	_uint			m_iNumVerticesZ = { 0 };
	_float			m_fVerticesItv = { 0 };


public:
	static CVIBuffer_Terrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext); // 바로 높이맵 파일을 받아오자. 10.02
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END