#pragma once

#include "PartObject.h"
#include "MapObject.h"
#include "Word.h"

BEGIN(Client)

class CWord_Container final : public CPartObject, public IInteractable
{
public:
	typedef struct tagWord_ContainerDesc : public CPartObject::PARTOBJECT_DESC
	{
		_uint iContainerIndex;
	}WORD_CONTAINER_DESC;
private:
	CWord_Container(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWord_Container(const CWord_Container& _Prototype);
	virtual ~CWord_Container() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


	// IInteractable을(를) 통해 상속됨
public:
	virtual void			Interact(CPlayer* _pUser) override;
	virtual _bool			Is_Interactable(CPlayer* _pUser) override;
	virtual _float			Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	
	CWord*	Get_Word() { return m_pMyWord; };
	void	Set_Word(CWord* _pWord);
	void	Pop_Word();
	
	void	Set_ControllerIndex(_uint _iIndex) { m_iControllerIndex = _iIndex; }
	void	Set_ContainerIndex(_uint _iIndex) { m_iContainerIndex = _iIndex; }

private:
	_uint		m_iControllerIndex = {};
	_uint		m_iContainerIndex = {};
	CWord*		m_pMyWord = nullptr;


public:
	static CWord_Container* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;

};

END