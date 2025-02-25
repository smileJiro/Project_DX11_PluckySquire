#pragma once
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Beam;
class CTexture;
END

BEGIN(Client)

class CEffect_Beam : public CPartObject
{
public:
	typedef struct tagEffectBeam : public CPartObject::PARTOBJECT_DESC
	{
		_bool	isConverge = { false };
		_bool	isRenderPoint = { false };
		_float	fConvergeSpeed = { 1.f };
		_float	fWidth = {1.f};
		_float	fPointSize = { 0.5f };
		_float3 vRandomMin = { 0.f, 0.f, 0.f };
		_float3 vRandomMax = { 0.f, 0.f, 0.f };
		_float4 vColor = {1.f, 1.f, 1.f, 1.f};
		_float4 vPointColor = { 1.f, 1.f, 1.f, 1.f };
		const _tchar* szPointTextureTag = L"";
		const _tchar* szTextureTag = L"";
		const _tchar* szBufferTag = L"";
	}EFFECTBEAM_DESC;
private:
	CEffect_Beam(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CEffect_Beam(const CEffect_Beam& _Prototype);
	virtual ~CEffect_Beam() = default;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void	Set_StartPosition(_fvector _vStartPosition);
	void	Set_EndPosition(_fvector _vEndPosition);

private:
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Beam* m_pBufferCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pPointTextureCom = { nullptr };

private:
	_bool	m_isConverge = { false };
	_bool	m_isRenderPoint = { false };
	_float	m_fWidth = 1.f;
	_float	m_fPointSize = 0.5f;
	_float	m_fConvergeSpeed = { 1.f };
	_float4 m_vColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float4 m_vPointColor = _float4(1.f, 1.f, 1.f, 1.f);

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components(const EFFECTBEAM_DESC* _pDesc);

public:
	static CEffect_Beam* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT Cleanup_DeadReferences() override;
};

END