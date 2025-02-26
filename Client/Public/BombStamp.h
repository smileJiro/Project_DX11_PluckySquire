#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CBombStamp :
    public CModelObject
{
public:
	typedef struct tagBOMBStampDesc : public CModelObject::MODELOBJECT_DESC
	{
	
	}BOMB_STAMP_DESC;

private:
	explicit CBombStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CBombStamp(const CBombStamp& _Prototype);
	virtual ~CBombStamp() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


	void Smash(_fvector v2DPosition);
private:

public:
	static CBombStamp* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END