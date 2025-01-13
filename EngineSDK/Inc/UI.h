#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	typedef struct tagUIDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float fX, fY, fSizeX, fSizeY;
		_uint iNumViewports, iCurViewportIdx;
		UI_DEPTHTYPE eDepthType;

		_wstring szTitle;
		_wstring szDesc;
		_float2 vTitleOffset;
		_float2 vDescOffset;

		_wstring strTexturePrototypeTag; // 해당 텍스쳐를 가지고 Texture Component 생성.
	}UI_DESC;
protected:
	CUI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CUI(const CUI& _Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta)override;
	virtual void Update(_float _fTimeDelta)override;
	virtual void Late_Update(_float _fTimeDelta)override;
	virtual HRESULT Render() override;
public:
	HRESULT Change_TextureComponent(_uint _iPrototypeLevelIndex, const _wstring& _strNewTexturePrototypeTag);
public: /* UI 기초 제공 기능 */
	virtual void Apply_UIPosition();		// 기본 좌상단 기준으로 정렬하지만, 특정 ui는 포지션을 중점 기준으로 정렬하게 세팅하기 위해 virtual
	void Apply_UIScaling();			// ui 스타일에 맞게 Transform Pos 조절.
	HRESULT Add_Child(CUI* _pChildUI);
	virtual void On_UI(); 			// 각각의 ui마다 종료 시, 처리해주어야 할 내용이 다르겠지. 해당 내용을  재정의 해서 사용. Begin, End 느낌.
	virtual void Off_UI();			// 각각의 ui마다 종료 시, 처리해주어야 할 내용이 다르겠지. 해당 내용을  재정의 해서 사용. Begin, End 느낌.

public: /* 지정된 시간동안 등속 이동 */
	void MoveToTarget_UI(_float _fTimeDelta);


public:	/* UI_Manager */
	virtual void MouseOn();
	virtual void MouseAway();		// 이전프레임 Target이었으나, 현재는 Target이 아닌 경우, 해당 함수 호출 됌.
									// 만약, 버튼 류 같은 경우 Target의 여부보다 클릭의 여부가 더 중요하다면, 해당 함수를 정의하지 않으면 되겠지.
	virtual void MouseLBUp();
	virtual void MouseLBDown();
	virtual void MouseLBClicked();	// 해당 ui 범위안에서 down과 up이 모두 이루어진

protected: /* Interaction */
	virtual void Check_MouseOn();			// 현재 자기 자신 위에 마우스가 올라와있는지 체크,
	virtual void Check_MouseOn_Circle();	// 현재 자기 자신 위에 마우스가 올라와있는지 체크,
protected:
	virtual void Child_Priority_Update(const float& _fTimeDelta);
	virtual void Child_Update(const float& _fTimeDelta);
	virtual void Child_Late_Update(const float& _fTimeDelta);
	virtual void Child_Render();

public:
	// Get
	const vector<CUI*>& Get_ChildUI() const { return m_ChildUIs; }
	_float2 Get_UIPosition() const { return _float2(m_fX, m_fY); }
	_float2 Get_FinalPos() const { return _float2(m_fFinalX, m_fFinalY); }
	_float2 Get_Size() const { return _float2(m_fSizeX, m_fSizeY); }
	_float2 Get_FinalSize() const { return _float2(m_fFinalSizeX, m_fFinalSizeY); }
	_bool Is_MouseOn() const { return m_isMouseOn; }
	_bool Is_LBDown() const { return m_bLBDown; }
	_uint Get_Depth() const { return m_iDepth; }
	UI_DEPTHTYPE Get_DepthType() { return m_eDepthType; }
	// Set
	void Set_UIPosition(_float2 _vUIPosition) { m_fX = _vUIPosition.x;  m_fY = _vUIPosition.y; }
	void Set_UISize(_float2 _vUISize) { m_fSizeX = _vUISize.x; m_fSizeY = _vUISize.y; }
	void Set_LBDown(_bool _b) { m_bLBDown = _b; }
	void Set_Depth(_uint _iDepth) { m_iDepth = _iDepth; }
	void Set_TitleFont(_wstring _strTitle) { m_strTitleText = _strTitle; }
	void Set_DescFont(_wstring _strDesc) { m_strDescText = _strDesc; }
	void Set_TitleFontOffset(_float2 _vOffsetPos) { m_vTitleFontOffset = _vOffsetPos; }
	void Set_DescFontOffset(_float2 _vOffsetPos) { m_vDescFontOffset = _vOffsetPos; }
	void Set_TargetUIPos(_float2 _vTargetUIPos);

public:
	void Set_TextureIndex(_uint _iTextureIndex);
protected:
	CTexture* m_pTextureCom = nullptr;
	_int m_iTextureIndex = 0;
protected:
	_wstring m_strTitleText;
	_float2 m_vTitleFontOffset = { 0.0f, 0.0f };
	_wstring m_strDescText;
	_float2 m_vDescFontOffset = { 0.0f, 0.0f };
protected:
	_float m_fX, m_fY, m_fSizeX, m_fSizeY; // 부모가 있다면, 자기 월드행렬을 구성 후 부모 행렬을 곱한다. ;
	_float m_fFinalX, m_fFinalY;
	_float m_fFinalSizeX, m_fFinalSizeY;

protected: /* ui 타겟 이동 관련. */
	_float2 m_vTargetUIPos = {};
	_float2 m_vTargetDir = {};
	_float m_fMoveTime = 0.15f;
	_float m_fUISpeed = 0.0f;


protected:
	_uint m_iNumViewports = 0; // 뷰포트의 개수
	_uint m_iCurViewportIdx = 0;
	_float m_fViewportX, m_fViewportY;
	_float4x4 m_ViewMatrix{}, m_ProjMatrix{};

protected:
	UI_DEPTHTYPE m_eDepthType = UI_DEPTHTYPE::LAST;
	_uint m_iDepth; // ui 소팅을 위한 깊이 값.

protected:
	_bool m_isMouseOn = false;
	_bool m_bLBDown = false; // UI 위에서 LBDown 된 적이 있다면 이를 기억, UI 매니저에서 Clicked 이벤트를 성립하기 위한 조건.
	
protected:
	CUI* m_pParentUI = nullptr;
	vector<CUI*> m_ChildUIs;

public:
	virtual HRESULT Ready_Components(UI_DESC* _pDesc);
public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
	virtual HRESULT Safe_Release_DeadObjects() override;

};

END