#pragma once
#include "CarriableObject.h"
#include "AnimEventReceiver.h"
#include "Interactable.h"


BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)
class CPlayer;
class CWord final :
	public CCarriableObject
{

	// 활성화된 단어는 캐리어블. 본인이 직접 렌더해야 한다.
	// 비활성화 된 단어는 Container에 수납되며, 본인의 텍스트를 대신 렌더해 준다.

public:
	enum WORD_TYPE
	{
		WORD_FULL,
		WORD_BIG,
		WORD_SMALL,
		WORD_CLOSE,
		WORD_OPEN,
		WORD_EMPTY,
		WORD_LAST
	};
	enum WORD_MODE
	{
		WORD_MODE_LAND,
		WORD_MODE_ENTER_SETUP,
		WORD_MODE_EXIT_SETUP,
		WORD_MODE_SETUP,
		WORD_MODE_LAST
	};

public :
	typedef struct tagWordDesc : CCarriableObject::CARRIABLE_DESC
	{
		WORD_TYPE					eType;
		ID3D11ShaderResourceView*	pSRV;                                               
		_float2						fSize;
		_wstring					strText;

	}WORD_DESC;


protected:
	explicit CWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CWord(const CWord& _Prototype);
	virtual ~CWord() = default;

public:
	HRESULT				Initialize(void* _pArg);

	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

	virtual HRESULT		Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

public:
	virtual void		Interact(CPlayer* _pUser) override;
	virtual _bool		Is_Interactable(CPlayer* _pUser) override;
	virtual _float		Get_Distance(COORDINATE _eCOord, CPlayer* _pUser) override;

public :
	_wstring	Get_Text()			{ return m_strText; }
	WORD_TYPE	Get_WordType()	{ return m_eWordType; }
	_bool		Is_LayedDown()		{ return m_bLaydown; }

	void		Set_LayDown(_bool _bLayDown) { m_bLaydown = _bLayDown; }

	// 튕겨나가거나, 들어가는 행위 작성
	void		Execute_WordMode_Action(_float _fTimeDelta);

	void		Set_OutputDirection(_float3 _fDir) { m_fOutputDir = _fDir; }
	void		Set_OutputSecond(_float _fSecond) { m_fOutputSecond = _fSecond; }
	void		Set_ContainorPosition(_float3 _fPos) { m_fContainorPosition = _fPos; }
	_bool		Is_Complete() { return m_isActionComplete; }

	WORD_MODE		Get_WordMode() { return m_eWordMode; }
	void			Set_WordMode(CWord::WORD_MODE _eMode = CWord::WORD_MODE_SETUP) 
	{ 
		m_eWordMode = _eMode;
		m_isActionComplete = false;
		m_fOutputSecond = 0.3f;
	}

public :
	HRESULT		Ready_Components();

protected:
	virtual void On_LayDownEnd(_fmatrix _matWoroverride) override;
	virtual void On_PickUpStart(CPlayer* _pPalyer, _fmatrix _matPlayerOffset)override;
	virtual void On_Land() override;
private :
	CVIBuffer_Rect*				m_pVIBufferCom = {};
	ID3D11ShaderResourceView*	m_pWordTexture = {};

	WORD_TYPE					m_eWordType;

	_wstring					m_strText;
	_float2						m_fSize;
	WORD_MODE					m_eWordMode = WORD_MODE_LAST;
	_bool						m_bLaydown = false;

	_float3						m_fOutputDir = {};
	_float						m_fOutputSecond = {};
	_float3						m_fContainorPosition = {};

	_bool						m_isActionComplete = false;

public:
	static	CWord*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;

};

END