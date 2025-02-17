#pragma once
#include "2DMapObject.h"

BEGIN(Engine)
class C2DModel;

END

BEGIN(Client)

class C2DMapWordObject final : public C2DMapObject
{
	enum WORD_ACTION
	{
		IMAGE_CHANGE,
		WORD_OBJECT_ACTIVE,
		WORD_ACTION_LAST
	};

public :
	typedef struct tagWordAction
	{
		_uint iContainerIndex;
		_uint iWordType;
		WORD_ACTION eAction;
		//ANY		tParam;
	};

public :
	typedef struct tag2DWordObjectDesc : public C2DMapObject::MAPOBJ_DESC
	{


	};
private:
	C2DMapWordObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	C2DMapWordObject(const C2DMapWordObject& _Prototype);
	virtual ~C2DMapWordObject() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;
	virtual HRESULT					Render_Shadow() override;


private :
	_wstring			m_strWordObjectTag;
	_uint				m_iModelIndex;
	vector <C2DModel*>	m_Models;

	




public:
	static C2DMapWordObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END
