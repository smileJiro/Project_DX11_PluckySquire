#pragma once
#include "Base.h"
#include "Narration_New.h"

BEGIN(Client)
class CNarration_New;

class CNarration_Manager final : public CBase
{
	DECLARE_SINGLETON(CNarration_Manager)


public:
	struct NarrationDialogData
	{

		_wstring strtext = TEXT(" ");		// 텍스트의 내용
		_wstring strSFX = TEXT("NOT");      // 노출 시킬 나레이션 사운드
		_wstring strSubSFX = TEXT("NOT");   // 노출 시킬 서브 사운드
		_float	fscale;						// 텍스트의 크기 변경
		_bool	isLeft;						// 왼쪽인가요 오른쪽인가요? 왼쪽이면 true
		_float2 fFontPos;						// X의 좌표
		_float	fwaitingTime;				// 완료 후 다음 나레이션까지의 대기 시간
		_float	fLineHieght;				// 문장과 문장 사이의 높이

		_float	fDelayNextLine;				// 페이드인이 완료 된 후 다음 라인으로 넘어가기까지의 시간
		_bool	isDirTurn = true;           // 책장을 넘기는 방향 true는 우측, false는 좌측
		_float3 fRGB = { 0.f, 0.f, 0.f };
		_bool   isFinishedThisLine = { false };
		_int    AnimationCount = { 0 };
		wstring strSectionid;       // 노출 시킬 섹션의 id
		wstring strAnimationid;     // 애니메이션의 id
		_float2 vPos;               // UI 오브젝트의 포지션
		_float  fWaitingTime;       // 완료 후 다음 애니메이션까지의 대기 시간

		_float2 vAnimationScale;
		_uint   iAnimationIndex;
		_bool   isLoop;

		C2DModel* pAnimModels;


	};

	struct NarrationData : public CUI::tagUIDesc
	{
		wstring		strSectionid;       // 노출 시킬 섹션의 id
		wstring		strid;              // 다이얼로그 ID
		LEVEL_ID	eCurlevelId = { LEVEL_END };
		_int		LineCount = { 0 };
		_bool		bTerminal = false;
		_int		AnimIndex = { 0 };
		_int		iduration = 1000;			// 지속 시간
		_int		ispeed;						// 다음 폰트 노출 시간
		_float		fFadeDuration;				// 페이드인이 완료될 때 까지의 시간

		NarrationDialogData NarAnim;

	};

private:
	CNarration_Manager();
	virtual ~CNarration_Manager() = default;

private:
	CGameInstance*				m_pGameInstance;
	map<_wstring, CNarration_New*>	m_Narrations;

	_bool						m_isPlaying = { false };
	_wstring					m_strCurNarrationIndex;



	

public:
	HRESULT								Level_Exit(_int iCurLevelID, _int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Logo_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);

	HRESULT								Set_PlayNarration(const _wstring& _strid);
	void								Set_Playing(_bool _isPlay) { m_isPlaying = _isPlay; }
	_bool								Get_Playing() { return m_isPlaying; }

	HRESULT								NarrationActive();

private:
	HRESULT								LoadFromJson(const wstring& filePath, LEVEL_ID _eLevelID);

public:
	virtual void Free() override;

};

END