#pragma once
#include "UI.h"
#include "Narration_Anim.h"

//BEGIN(Engine)
//class CShader;
//class CModel;
//class CVIBuffer_Collider;
//END


BEGIN(Client)
class CNarration : public CUI
{
public:
	struct TextTokens
	{
		_wstring    strText;
		_float      fScale = { 1.f };
        _float      fY = { 0.f };
        _float      fX = { 0.f };
	};

    struct NarrationAnimation
    {
        wstring strSectionid;       // 노출 시킬 섹션의 id
        wstring strAnimationid;     // 애니메이션의 id
        _float2 vPos;               // UI 오브젝트의 포지션
        _float  fWaitingTime;       // 완료 후 다음 애니메이션까지의 대기 시간
        _float2 vAnimationScale;
        _uint   iAnimationIndex;
    };

    struct NarrationDialogData
    {
		_int iduration = 1000;      // 지속 시간
		_int ispeed;                // 다음 폰트 노출 시간
		_wstring strtext;           // 텍스트의 내용
		_float fscale;              // 텍스트의 크기 변경
        _bool isLeft;                // 왼쪽인가요 오른쪽인가요? 왼쪽이면 true
        _float2 fpos;               // X의 좌표
        _float fwaitingTime;        // 완료 후 다음 애니메이션까지의 대기 시간

        _int    AnimationCount = { 0 };
        vector<NarrationAnimation> NarAnim;

    };

    struct NarrationData : public tagUIDesc
    {
        wstring strSectionid;       // 노출 시킬 섹션의 id
        wstring strid;              // 다이얼로그 ID
        LEVEL_ID eCurlevelId = { LEVEL_END };
        _int     LineCount = { 0 };
        _int        AnimIndex = { 0 };
        vector<NarrationDialogData> lines;
    };





public:
    explicit CNarration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit CNarration(const CNarration& Prototype);
    virtual ~CNarration() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta);
    virtual HRESULT Render() override;

private:
    HRESULT     LoadFromJson(const std::wstring& filePath); // 데이터 로드
    HRESULT      DisplayText(_float2 _vRTSize); // 타이핑 되게하자.
    void        NextDialogue(_float2 _RTSize);
    CNarration_Anim* GetAnimationObjectForLine(_int iLine);

private:
    //DialogData          m_DialogData;   // 현재 다이얼로그 데이터
    _int                m_iCurrentLineIndex = 0; // 현재 출력 중인 라인의 인덱스 (id)
    _tchar			    m_tDialogIndex[MAX_PATH] = {};

    NarrationData            m_NarrationData;
    vector<NarrationData>  m_NarrationDatas;

    _float                  m_fLineHeight = { 70.f };
    _int                    m_RemainWord = { 0 };

    _int m_iCurrentLine = 0;      // 현재 화면에 노출할 대화 라인 인덱스
    _float m_fTextAlpha = 0.f;      // 현재 텍스트의 알파값(0.0~1.0)
    _float m_fFadeDuration = 3.f;  // Fadein 효과에 걸리는 시간 (초)
    _float m_fDelayBetweenLines = 5.f; // 라인 교체 전 대기 시간 (초)
    _float m_fFadeTimer = 0.f;      // fade-in 진행 타이머
    _float m_fDelayTimer = 0.f;     // 라인 대기 타이머
    _bool  m_bAnimationStarted = false; // 현재 라인에 대해 애니메이션이 시작되었는지 여부
    CNarration_Anim* m_pCurrentAnimObj;
    vector<CNarration_Anim*> m_vAnimObjectsByLine;

protected:
    virtual HRESULT Ready_Components() override;


public:
    static CNarration* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    HRESULT      Cleanup_DeadReferences() override;

private:
    void PaseTokens(const _wstring& _Text, vector<TextTokens>& _OutToken);

};
END
