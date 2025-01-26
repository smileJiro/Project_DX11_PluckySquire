#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END


BEGIN(Client)

class CDialog : public CUI
{

    enum LOC { LOC_DEFAULT, LOC_MIDDLE, LOC_MIDLEFT, LOC_MIDRIGHT, LOC_END };
    // Default : 가운데 아래
    // Middle : 정 가운데
    // MidLeft : 가운데 살짝 좌측
    // MidRight : 가운데 살짝 우측

public:
    struct Animation
    {
        _int duration = 1000;   // 지속 시간 (밀리초)
        _int speed = 100;      // 속도 (타자기 효과 등)
    };

    struct DialogLine
    {
        wstring Talker;  // 말하는 놈 : 이름 및 초상화를 설정하자.
        wstring text;     // 대화 내용
        _int   BG;     // 배경
        LOC  location = LOC_DEFAULT;    // 위치 설정
        Animation animation;   // 애니메이션 정보
    };

    struct DialogData
    {
        wstring id;                     // 다이얼로그 ID
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

private:
    HRESULT     LoadFromJson(const std::wstring& filePath); // 데이터 로드
    HRESULT      DisplayTextWithAnimation(); // 타이핑 되게하자.

private:
    DialogData          m_DialogData;   // 현재 다이얼로그 데이터
    _int                m_iCurrentLineIndex = 0; // 현재 출력 중인 라인의 인덱스 (id)

    vector<DialogData>  m_DialogDatas;

    _bool               m_isOpenDialogue = { true };

    vector<DialogData>         Get_Dialogues() { return m_DialogDatas; }

protected:
    virtual HRESULT Ready_Components() override;

public:
    static CDialog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    HRESULT      Cleanup_DeadReferences() override;

};

END
