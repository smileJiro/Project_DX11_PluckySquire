#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{

	// 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
	// 기능을 인스턴스화 하기 위하여 만들어두는 틀

	template<typename T>
	void	Safe_Delete(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	void	Safe_Delete_Array(T& Pointer)
	{
		if (nullptr != Pointer)
		{
			delete [] Pointer;
			Pointer = nullptr;
		}
	}

	template<typename T>
	unsigned int Safe_AddRef(T& pInstance)
	{
		unsigned int		iRefCnt = 0;

		// pInstance 객체가 nullptr이 아닐때, 레퍼런스 카운트를 증가 후, 해당 결과를 리턴한다.
		if (nullptr != pInstance)		
			iRefCnt = pInstance->AddRef();		

		return iRefCnt;
	}

	template<typename T>
	unsigned int Safe_Release(T& pInstance)
	{
		unsigned int		iRefCnt = 0;

		if (nullptr != pInstance)
		{
			iRefCnt = pInstance->Release();

			if (0 == iRefCnt)
				pInstance = nullptr;
		}

		return iRefCnt;
	}

	//HRESULT ReadJsonFile(const _char* strJsonFilePath, json* pOut);
	//HRESULT SaveJsonFile(const _char* strJsonFilePath, const json& pJson);
}


PxFilterFlags TWFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
							 PxFilterObjectAttributes attributes1, PxFilterData filterData1,
							 PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize
);

#endif // Engine_Function_h__
