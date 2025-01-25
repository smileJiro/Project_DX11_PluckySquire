#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Clasee)
class CDialog final : public CUI
{
private:
	explicit CDialog(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDialog(const CDialog& _Prototype);
	virtual ~CDialog() = default;

public:
	struct ImagePos
	{

	};
		


	//typedef struct DIALOGUE
	//{
	//	string	sDialogueIndex; // 다이얼로그의 고유 인덱스 값
	//	string	sName; // 말하는 캐릭터
	//	//_uint	iNpcMainKey; // 말하는 캐릭터의 메인 키값
	//	//_uint	iNpcSubKey = 0; // 말하는 캐릭터의 서브 키값, 없으면 0으로 고정
	//	_uint	iBackGround = 0; // 대사의 배경 인덱스 없으면 기본 백그라운드
	//	_uint	iDialogueFace = 0; // 말하는 캐릭터의 초상화 / 없으면 0
	//	HEIGHT	eHeight = HEIGHT_BASIC; // 다이얼로그 창의 세로 위치 / 없으면 기본값
	//	WIDTH	eWidth = WIDTH_BASIC; // 다이얼로그 창의 가로 위치 / 없으면 기본값
	//
	//	string	sDialogueText; // 대사
	//} DialInfo;

};

END