// 명령어			옵션		원본 파일의 위치					사본 파일을 저장할 위치
	
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\Debug
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\Release
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Camera\Bin\Debug
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Camera\Bin\Release
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Map\Bin\Debug
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Map\Bin\Release
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Animation\Bin\Debug
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Animation\Bin\Release
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Effect\Bin\Debug
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool_Effect\Bin\Release
xcopy			/y			.\Engine\Bin\Engine.lib			.\EngineSDK\Lib\
xcopy			/y			.\Engine\ThirdPartyLib\*.lib		.\EngineSDK\Lib\

xcopy			/y/s		.\Engine\Public\*.*				.\EngineSDK\Inc\
xcopy			/y/s		.\Engine\Bin\ShaderFiles\*.*	.\EngineSDK\hlsl\
