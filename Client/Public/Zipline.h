#pragma once
#include "ModelObject.h"
#include "Interactable.h"

BEGIN(Client)

class CZipline : public CModelObject, public IInteractable
{
public:
	typedef struct tagZiplineDesc : public CModelObject::MODELOBJECT_DESC
	{
		_wstring  strSectionTag = L"";
	}ZIPLINE_DESC;

protected:
	CZipline(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZipline(const CZipline& _Prototype);
	virtual ~CZipline() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Interact(CPlayer* _pUser) override;
	virtual _bool	Is_Interactable(CPlayer* _pUser) override;
	virtual _float	Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;

protected:
	_float2			m_fZiplineTime = { 5.5f, 0.f };
	_bool			m_isRideDown = { false };

	CPlayer*		m_pRidingObject = { nullptr };

	_float3			m_vStartPos = {};
	_float3			m_vDestination = {};

private:
	void			Ride_Down(_float _fTimeDelta);

public:
	static CZipline*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END
