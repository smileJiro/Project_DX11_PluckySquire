#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END


BEGIN(Client)
class CDialogue final : public CUI
{
public:
	typedef struct DIALPOS
	{
		_float	fX;
		_float	fY;
	}DialPos;


	typedef struct DIALOGUEINFORMATION
	{
		string	sTalker;
		string	sText;
		_uint	iFace;
		_uint	iBG;
		DialPos vPos;
	}DialInfo;

	typedef struct DIALOG
	{
		string	sID; // 다이얼로그 고유 인덱스
		_uint	iNpcID;
		_uint	iNpcSecID = 0;
		vector<DialInfo>	vDialogue;
	}Dialog;


protected:
	explicit CDialogue(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDialogue(const CDialogue& _Prototype);
	virtual ~CDialogue() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	vector<Dialog>			Get_Dialogues() { return m_Dialogues; }
	void					Display_Dialogues();

protected:
	virtual HRESULT			Ready_Components() override;

public:
	static CDialogue*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

private:
	vector<Dialog>			m_Dialogues;

};
END
