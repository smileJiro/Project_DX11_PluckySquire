#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Particle : public CVIBuffer_Instance
{
private:
	class CVIBuffer_Mesh_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	class CVIBuffer_Mesh_Particle(const CVIBuffer_Mesh_Particle& _Prototype);
	virtual ~CVIBuffer_Mesh_Particle() = default;

public:
	HRESULT Initialize_Prototype(ifstream& _inFile, const json& _jsonBufferInfo, _fmatrix _PreTransformMatrix);
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;

	virtual void	Reset_Buffers() override;

public:
	// Get
	_char* Get_Name() { return m_szName; }
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }

	// Set
	void Set_Name(string _strName) { strcpy_s(m_szName, _strName.c_str()); }

private:
	_float4x4			 m_PreTransformMatrix = {};

private:
	/* Mesh Data */
	_char m_szName[MAX_PATH] = ""; // Mesh의 이름은 추후 중요하게 사용 된다.
	_uint m_iMaterialIndex = 0; // 내가 렌더링에 사용하고자하는 Material Index;


private:
	VTXMESHINSTANCE* m_pInstanceVertices = { nullptr };

private:
	HRESULT Ready_VertexBuffer(ifstream& _inFile, _fmatrix _PreTransformMatrix);
	void			  Set_Instance(_int _iIndex);
	void			  Set_Position(_int _iIndex);
	HRESULT			 Initialize_Particles();

public:
	static CVIBuffer_Mesh_Particle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, ifstream& _inFile, const json& _jsonBufferInfo, _fmatrix _PreTransformMatrix);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;

#ifdef _DEBUG
public:
	virtual void		 Tool_Setting() override;
	virtual void		 Tool_Reset_Instance() override;
	virtual void		 Tool_Reset_Buffers() override; // Count 자체가 바뀌어버린 경우

	virtual void		 Tool_Update(_float _fTimeDelta) override;
	virtual HRESULT		 Save_Buffer(json& _jsonBufferInfo) override;

private:
	_float2* m_pTexcoords = { nullptr };
	_float3* m_pTangents = { nullptr };
	_uint	 m_iNumFaces = { 0 };
#endif

};

END