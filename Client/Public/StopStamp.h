#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CStopStamp :
    public CModelObject
{
public:
	typedef struct tagStopStampDesc : public CModelObject::MODELOBJECT_DESC
	{
	
	}STOP_STAMP_DESC;

private:
	explicit CStopStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CStopStamp(const CStopStamp& _Prototype);
	virtual ~CStopStamp() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	void Place_Stopper(_fvector v2DPosition);
private:

public:
	static CStopStamp* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END