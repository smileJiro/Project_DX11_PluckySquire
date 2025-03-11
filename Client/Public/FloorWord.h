#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)

class CPrintFloorWord;
class CFloorWord : public CUI
{
//public:
//	typedef struct tagFloorWordDesc : public CUI::UIOBJECT_DESC
//	{
//		const _tchar* poapdfkpasdfko
//
//	};
public:

	struct Highlight
	{
		wstring strSubstring;
		float fDefultSize;
		float fHighlightSize;
	};


	struct FLOORTEXT
	{
		_wstring strSection;	// 노출할 섹션
		float fPosX;			// X
		float fPosY;			// Y
		wstring strText;		// 노출할 텍스쳐
		Highlight tHightLight;	// 하이라이트 인데 미사용
		wstring strSFX = { TEXT("NOTWORD") };

	};


protected:
	explicit CFloorWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CFloorWord(const CFloorWord& _Prototype);
	virtual ~CFloorWord() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual HRESULT			Initialize_Child(void* _pArg);
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CFloorWord*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	HRESULT					Load_Json(const wstring& filePath, const _wstring& _strLayerTag);


private:
	_bool					m_isMake = { false };

};

END
