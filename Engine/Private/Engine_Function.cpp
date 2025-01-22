#include "Engine_Defines.h"

PxFilterFlags TWFilterShader(PxFilterObjectAttributes Attribute_0, PxFilterData FilterData_0, PxFilterObjectAttributes Attribute_1, PxFilterData FilterData_1, PxPairFlags& PairFlags, const void* ConstantBlock, PxU32 ConstantBlockSize)
{
	// 트리거 처리
	if (PxFilterObjectIsTrigger(Attribute_0) || PxFilterObjectIsTrigger(Attribute_1))
	{
		PairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}

	// 사용자 정의 조건에 따른 충돌 활성화
	if ((FilterData_0.word0 & FilterData_1.word1) && (FilterData_1.word0 & FilterData_0.word1))
	{
		PairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_PERSISTS | PxPairFlag::eNOTIFY_TOUCH_LOST;
		return PxFilterFlag::eDEFAULT;
	}

	// 기본적으로 충돌 비활성화
	return PxFilterFlag::eSUPPRESS;
}
