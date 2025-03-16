#pragma once
#include "ModelObject.h"
#include "Stoppable.h"
#include "SlipperyObject.h"

BEGIN(Engine)
class CModelObject;
END
BEGIN(Client)
class CTiltSwapCrate :
	public CSlipperyObject
{
public:

	enum FLIP_STATE
	{
		FLIP_BOTTOM,
		FLIP_TOP,
		FLIP_LAST
	};
	typedef struct tagTiltSwapCrateDesc : public CSlipperyObject::SLIPPERY_DESC
	{
		FLIP_STATE eFlipState = FLIP_TOP;
	} TILTSWAPCRATE_DESC;
public:
	CTiltSwapCrate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTiltSwapCrate(const CTiltSwapCrate& _Prototype);
	virtual ~CTiltSwapCrate() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;
public:
	void Flip();
	void Set_FlipState(FLIP_STATE _eState);
	void Decalcomani(_bool _bXAxis);
private:
	HRESULT Ready_Parts();
	_vector Get_CenterPos();
	void Set_Position_ByCenter(_fvector _vCenterPos);
private:
	_float2 m_vColliderHalfSize = { 125.f, 120.f };
	FLIP_STATE m_eFlipState = FLIP_LAST;
	CModelObject* m_pBody = nullptr;
public:
	static CTiltSwapCrate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END