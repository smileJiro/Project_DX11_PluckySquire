#pragma once
#include "ModelObject.h"
#include "Flippable.h"

BEGIN(Client)
class CSneak_FlipObject : public CModelObject, CFlippable
{
public:
	typedef struct tagFlipObject : public CModelObject::MODELOBJECT_DESC
	{
		_int _iInitAnim = -1;
		_int _iFlipAnim1 = -1;
		_int _iFlipAnim1End = -1;
		_int _iFlipAnim2 = -1;
		_int _iFlipAnim2End = -1;
	}FLIPOBJECT_DESC;

protected:
	CSneak_FlipObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_FlipObject(const CSneak_FlipObject& _Prototype);
	virtual ~CSneak_FlipObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);
	virtual void Flip();

public:
	static CSneak_FlipObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END