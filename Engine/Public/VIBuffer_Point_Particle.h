#pragma once
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Particle : public CVIBuffer_Instance
{
private:
	CVIBuffer_Point_Particle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CVIBuffer_Point_Particle(const CVIBuffer_Point_Particle& _Prototype);
	virtual ~CVIBuffer_Point_Particle() = default;

public:
	HRESULT Initialize_Prototype(const json& _jsonBufferInfo);
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;

private:
	VTXPOINTINSTANCE* m_pInstanceVertices = { nullptr };

	// UV 정보
	// 모든 Atlas는 위에서 아래, 좌에서 우로 향한다는 가정.
	// 모든 Texture의 크기는 같다는 가정
	_bool			  m_isAnim = { false };
	_bool			  m_isRandomUV = { false };
	_float			  m_fAnimTime = { 0.f };
	_float			  m_fStartIndex = { 0.f };
	_float			  m_fAnimCount = { 1.f };
	_float2			  m_vUVPerAnim = { 1.f, 1.f };

private:
	void			  Set_UV(_Out_ _float4* _pOutUV, _float _fIndex);
	void			  Set_Position(_int _iIndex);
	void			  Set_Instance(_int _iIndex);

	HRESULT			 Initialize_Particles();

public:
	static CVIBuffer_Point_Particle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const json& _jsonBufferInfo);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;

#ifdef _DEBUG
public:
	virtual void		 Tool_Setting() override;
	virtual void		 Tool_Reset_Instance() override;
	virtual void		 Tool_Reset_Buffers() override; // Count 자체가 바뀌어버린 경우

	virtual void		 Tool_Update(_float _fTimeDelta) override;
	virtual HRESULT		 Save_Buffer(json& _jsonBufferInfo) override;
	

public:
	HRESULT Initialize_Prototype(_uint _iNumInstances);
public:
	static CVIBuffer_Point_Particle* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iNumInstances);

#endif

};
END
