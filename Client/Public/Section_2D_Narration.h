#pragma once
#include "Section_2D.h"
BEGIN(Client)

class CSection_2D_Narration final : public CSection_2D
{
public :
	typedef struct tagSection2DDesc : public CSection_2D::SECTION_2D_DESC
	{
	}SECTION_2D_NARRATION_DESC;

private:
	CSection_2D_Narration(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType);
	virtual ~CSection_2D_Narration() = default;

public:
	HRESULT						Initialize(void* _pDesc);
	HRESULT						Import(void* _pDesc);
public:

	virtual HRESULT				Section_AddRenderGroup_Process() override;
	virtual HRESULT				Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT				Remove_GameObject_ToSectionLayer(CGameObject* _pGameObject) override;
	HRESULT						Start_Narration();



	// 맵 연결 생각안해놨는데, 일단 해보자.

public:
	static CSection_2D_Narration* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext,  SECTION_2D_PLAY_TYPE _ePlayType, void* _pDesc);
	void Free() override;

private:
	_bool		m_FirstPlay[8] = { false, false, false, false, false, false, false, false };
};

END