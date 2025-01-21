#pragma once
#include "ContainerObject.h"

BEGIN(physx)
class CRigidBody;
END

BEGIN(Client)
class CCharacter abstract :
    public CContainerObject
{
public:
	typedef struct tagCharacterDesc : public CContainerObject::CONTAINEROBJ_DESC
	{
		
	}CHARACTER_DESC;

protected:
	CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCharacter(const CCharacter& _Prototype);
	virtual ~CCharacter() = default;

public:


private:

public:	
	virtual void Free() override;
};

END