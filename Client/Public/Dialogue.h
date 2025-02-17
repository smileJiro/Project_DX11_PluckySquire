#pragma once
#include "UI.h"
#include "Portrait.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END


BEGIN(Client)

class CPortrait;

class CDialog : public CUI
{
public:
    enum LOC { 
        LOC_MIDDOWN, 
        LOC_MIDHIGH, // Default 인자
        LOC_MIDLEFT, 
        LOC_MIDRIGHT, 
        LOC_LEFTDOWN,
        LOC_LEFTHIGH,
        LOC_RIGHTHIGH,
        LOC_RIGHDOWN,
        LOC_VERYMIDHIGH,
        LOC_END };

    enum PORTRAITNAME
    {
        PORTRAITNAME_VIOLET,                // 마법사
        PORTRAITNAME_THRASH,                // 트롤
        PORTRAITNAME_MARTINA,               // 상점
        PORTRAITNAME_PIP,                   // 쥐
        PORTRAITNAME_PAGE,                  // 책벌레
        PORTRAITNAME_HUMGRUMP,              // 험그럼프
        PORTRAITNAME_ROLA,                  // 엄마
        PORTRAITNAME_MOONBEARD,             // 달수염
        PORTRAITNAME_MOONBEARD_MONO,        // 회색 달수염
        PORTRAITNAME_MOONBEARD_RUNETOOTH,   // 이어폰낀 달수염
        PORTRAITNAME_MINIBEARD,             // 조그만 달수염
        PORTRAITNAME_MINIBEARD_MONO,        //조그만 회색 달수염
        PORTRAITNAME_KRONG,                 // 쓰레쉬 할아버지
        PORTRAITNAME_KRONG_GHOST,           // 죽은 쓰레쉬 할아버지
        PORTRAITNAME_BENNYBEE,              // 미니게임에 있는 벌
        PORTRAITNAME_CHROMA,                // 여왕님
        PORTRAITNAME_TUMBA,                 // 애기 홍학(삽파는아이)

        PORTRAITNAME_DEFAULT,
        PORTRAITNAME_END
    };


public:
    struct Animation
    {
        _int duration = 1000;   // 지속 시간
        _int speed = 350;      // 속도
    };

    struct DialogLine
    {

        wstring Talker;  // 말하는 놈 : 이름 및 초상화를 설정하자.
        wstring text;     // 대화 내용
        _int   BG;     // 배경
        _int Red;
        _int Green;
        _int Blue;

        _bool is2D;
        _bool isPortrait;

        _float3 vFontColor = _float3(0.f, 0.f, 0.f);
        
        LOC  location = LOC_MIDHIGH;    // 위치 설정
        PORTRAITNAME portrait = PORTRAITNAME_DEFAULT;
        Animation animation;   // 애니메이션 정보
    };

    struct DialogData
    {
        wstring id;                     // 다이얼로그 ID
        wstring Section;                // 노출되어야할 섹션
        vector<DialogLine> lines;       // 대화 내용 목록
    };

public:
    explicit CDialog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CDialog(const CDialog& Prototype);
    virtual ~CDialog() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta);
    virtual HRESULT Render() override;

public:
    CPortrait* Get_Portrait() { return m_pPortrait; }
    void        Set_Portrait(CPortrait* _Portrait) { m_pPortrait = _Portrait; Safe_AddRef(_Portrait); }

private:
    HRESULT     LoadFromJson(const std::wstring& filePath); // 데이터 로드
    HRESULT      DisplayText(_float2 _vRTSize); // 타이핑 되게하자.
    void        NextDialogue(_float2 _RTSize);
    void        FirstCalPos(_float2 _RTSize);
    void        isOpenPanel(_tchar* _DialogId);

private:
    DialogData          m_DialogData;   // 현재 다이얼로그 데이터
    _int                m_iCurrentLineIndex = 0; // 현재 출력 중인 라인의 인덱스 (id)
    _tchar			    m_tDialogIndex[MAX_PATH] = {};
    _float2             m_vCurPos = { 0.f, 0.f };
    _bool               m_isFirstRefresh = { false };
	_float2             m_vDisplay3DSize = { 0.f, 0.f };
	_float2             m_vDisplay2DSize = { 0.f, 0.f };
    _bool               m_isAddSectionRender = { false };
    _tchar              m_strDialogSection[MAX_PATH] = {};

    _float3             m_vFontColor = _float3{ 0.f, 0.f, 0.f };

    CPortrait*          m_pPortrait = { nullptr };

    //vector<DialogData>  m_DialogDatas;

    _bool               m_isOpenDialogue = { true };

   // vector<DialogData>         Get_Dialogues() { return m_DialogDatas; }

protected:
    virtual HRESULT Ready_Components() override;

public:
    static CDialog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    HRESULT      Cleanup_DeadReferences() override;

};

END
