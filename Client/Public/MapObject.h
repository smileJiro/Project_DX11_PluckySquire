#pragma once
#include "ModelObject.h"
#include "3DModel.h"


BEGIN(Client)

class CMapObject abstract :  public CModelObject
{
public :
	typedef struct tagMapObjectDesc : public CModelObject::MODELOBJECT_DESC
	{
	}MAPOBJ_DESC;
protected:
	CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMapObject(const CMapObject& _Prototype);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;

protected :
	wstring							m_strModelName = L"";
public:
	virtual void			Free() override;
};
END
