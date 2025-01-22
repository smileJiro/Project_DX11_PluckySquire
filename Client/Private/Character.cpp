#include "stdafx.h"
#include "Character.h" 

CCharacter::CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
	
}

CCharacter::CCharacter(const CCharacter& _Prototype)
	: CContainerObject(_Prototype)
{
}


void CCharacter::Free()
{
	__super::Free();
}
