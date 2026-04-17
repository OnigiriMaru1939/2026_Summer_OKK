#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "FileManager.h"
#include "Fps.h"

Application::Application()
{
	SetWindowText("lumina's Guidance");
	SetGraphMode(SCREEN_WID, SCREEN_HIG, 32);
	ChangeWindowMode(false);
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

#ifdef _WIN64
	Live2D_SetCubism4CoreDLLPath(TEXT("CubismSdkForNative-5-r.4.1/Core/dll/windows/x86_64/Live2DCubismCore.dll"));
#else
	Live2D_SetCubism4CoreDLLPath(TEXT("CubismSdkForNative-5-r.4.1/Core/dll/windows/x86/Live2DCubismCore.dll"));
#endif

	DxLib_Init();

	fileMng = std::make_unique<FileManager>();
	sceneMng = std::make_unique<SceneManager>(*fileMng);
}

Application::~Application()
{
	// デストラクト明示
	sceneMng.reset();
	fileMng.reset();

	DxLib_End();
}

void Application::Run()
{
	Fps fps;
	// エンターキーとエスケープキーの同時押しで強制終了
	// どちらか片方だけだと誤操作で終了してしまう可能性あり
	while (ProcessMessage() == 0 && !sceneMng->GetExit() && (!CheckHitKey(KEY_INPUT_RETURN) || !CheckHitKey(KEY_INPUT_ESCAPE)))
	{
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();

		fps.Update();
		Update();

		Draw();
#ifdef _DEBUG
		fps.Draw();
#endif // DEBUG_

		ScreenFlip();

		fps.Wait();
	}
}

void Application::Update()
{
	InputManager::GetInstance().Update();
	sceneMng->Update();
}

void Application::Draw()
{
	sceneMng->Draw();
}
