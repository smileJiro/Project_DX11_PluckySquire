#pragma once
#include "Section_2D_PlayMap.h"
BEGIN(Client)
class CSection_2D_PlayMap_Book final : public CSection_2D_PlayMap
{
private:
	CSection_2D_PlayMap_Book(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection_2D_PlayMap_Book() = default;

public:
	static CSection_2D_PlayMap_Book* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson);
	void Free() override;
};

END