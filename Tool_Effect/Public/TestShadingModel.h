#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class CTexture;
END

class CTestShadingModel : public CModelObject
{
	typedef struct tagSingleFresnel
	{
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };

		_float	fExp = 2.f;
		_float	fBaseReflect = 1.0f;
		_float	fStrength = 1.0f;
		_float	fDummy;

	} SINGLEFRESNEL_INFO;

	typedef struct tagColorBuffer
	{
		_float4 vDiffuseColor = { 1.f, 1.f, 1.f, 1.f };
		_float4 vBloomColor = { 1.f, 1.f, 1.f, 1.f};
		_float4	vSubColor = { 1.f, 1.f, 1.f, 1.f };
		_float4 vInnerColor = { 1.f, 1.f, 1.f, 1.f };
	} COLORINFOBUFFER;
private:
	CTestShadingModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTestShadingModel(const CTestShadingModel& _Prototype);
	virtual ~CTestShadingModel() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	SINGLEFRESNEL_INFO		m_tFresnel = {};
	COLORINFOBUFFER			m_tColor = {};
	ID3D11Buffer*			m_pFresnelsBuffer = { nullptr };
	ID3D11Buffer*			m_pColorBuffer = { nullptr };

	CTexture* m_pMainTextureCom = { nullptr };
	CTexture* m_pNoiseTextureCom = { nullptr };

	_float			m_fTime = { 0.f };
	_float2			m_vDiffuseScaling = { 1.f, 1.f };
	_float2			m_vNoiseScaling = { 1.f, 1.f };
public:
	static CTestShadingModel* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

