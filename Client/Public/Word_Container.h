#pragma once

#include "ContainerObject.h"
#include "MapObject.h"
#include "Word.h"

BEGIN(Client)

class CWord_Container final : public CModelObject
{
public:
	typedef struct tagWord_ContainerDesc : public CContainerObject::CONTAINEROBJ_DESC
	{

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


protected:

private:
	CWord* m_pMyWord = nullptr;

public:
	static CWord_Container* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END