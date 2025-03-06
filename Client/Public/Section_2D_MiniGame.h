#pragma once
#include "Section_2D.h"
BEGIN(Client)
class CSection_2D_MiniGame abstract : public CSection_2D
{
public:
	enum SECTION_2D_MINIGAME_TYPE
	{
		WORD,
		PIP,
		BOXING,
		RHYTHM,
		PUYOPUYO,
		BOW,
		FANTASYCARDGAME,
		SECTION_2D_MINIGAME_LAST
	};

public:
	typedef struct tagSection2DDesc : public CSection_2D::SECTION_2D_DESC
	{
		SECTION_2D_MINIGAME_TYPE	eMiniGameType;
		_wstring					strMiniGameName;
	}SECTION_2D_MINIGAME_DESC;

protected:
	CSection_2D_MiniGame(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, SECTION_2D_RENDER_TYPE _eRenderType);
	virtual ~CSection_2D_MiniGame() = default;


public:
	virtual HRESULT						Initialize(void* _pDesc);
	virtual HRESULT						Ready_Objects(void* _pDesc) { return S_OK; };

public:
	virtual HRESULT						Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT						Section_AddRenderGroup_Process() override;



	const _wstring						Get_MiniGameName() { return m_strMiniGameName; }
	SECTION_2D_MINIGAME_TYPE			Get_MiniGameType() { return m_eMiniGameType; }

	void								Set_MiniGameName(const _wstring& _strName) { m_strMiniGameName = _strName; }
	void								Set_MiniGameType(SECTION_2D_MINIGAME_TYPE _eType) { m_eMiniGameType = _eType; }		


protected:
	SECTION_2D_MINIGAME_TYPE	m_eMiniGameType	= SECTION_2D_MINIGAME_LAST;
	_wstring					m_strMiniGameName	= L"";

public:
	void Free() override;
};

END