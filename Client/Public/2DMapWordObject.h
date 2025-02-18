#pragma once
#include "2DMapObject.h"

BEGIN(Engine)
class C2DModel;

END

BEGIN(Client)

class C2DMapWordObject final : public C2DMapObject
{
	enum WORD_ACTION_TYPE
	{
		IMAGE_CHANGE,
		WORD_OBJECT_ACTIVE,
		ANIMATION_CHANGE,
		POSITION_CHANGE_X,
		POSITION_CHANGE_Y,
		WORD_ACTION_LAST
	};

public :
	typedef struct tagWordAction
	{
		_uint				iControllerIndex;
		_uint				iContainerIndex;
		_uint				iWordType;
		WORD_ACTION_TYPE	eAction;
		any					anyParam;
	} WORD_ACTION;

public :
	typedef struct tag2DWordObjectDesc : public C2DMapObject::MAPOBJ_DESC
	{
		json WordObjectJson;

	}WORD_OBJECT_DESC;
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

	virtual HRESULT					Action_Execute(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex);
	const WORD_ACTION*				Find_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex);
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();
private :
	_wstring				m_strWordObjectTag;
	_uint					m_iModelIndex = 0 ;
	vector <_wstring>		m_ModelNames;
	vector <WORD_ACTION>	m_Actions;

	

	_bool					m_IsWordActive = true;


public:
	static C2DMapWordObject*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END
