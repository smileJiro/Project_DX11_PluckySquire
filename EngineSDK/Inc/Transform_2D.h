#pragma once
#include "Transform.h"
BEGIN(Engine)
class ENGINE_DLL CTransform_2D final : public CTransform
{
public:
	typedef struct tagTransform2DDesc : public CTransform::TRANSFORM_DESC
	{
		_float fInitialRotation = 0.f;
	}TRANSFORM_2D_DESC;

private:
	CTransform_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTransform_2D(const CTransform& _Prototype);
	virtual ~CTransform_2D() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);

public: /* 2D는 제한해야하는 Transform의 공통 함수. */
	virtual _bool			Go_Straight(_float _fTimeDelta) override;
	virtual _bool			Go_Backward(_float _fTimeDelta) override;
	virtual void			RotationXYZ(const _float3& _vRadianXYZ) override		{ RotationZ(_vRadianXYZ.z); }
	virtual void			RotationQuaternion(const _float3& _vRadianXYZ) override { RotationZ(_vRadianXYZ.z); }

	void			LookDir(_fvector _vDir) ;

private:
	_float2 m_vPosition = {};

public:
	static CTransform_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};
END
