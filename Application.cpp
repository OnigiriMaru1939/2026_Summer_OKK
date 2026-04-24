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
	static float x = 0;
	static float y = 0;
	float moveSpeed = 5.0f;
	sceneMng->Draw();
	InputManager::GetInstance().DrawDebug(0, 40);
	if (InputManager::GetInstance().IsActionPressed(JUMP_ACTION))
	{
		DrawString(SCREEN_WID / 2, 60, "Jump Pressed", 0xffffff);
	}
	if (InputManager::GetInstance().IsActionPressed(MOVE_UD_ACTION))
	{
		DrawString(SCREEN_WID / 2, 80, "Move Down or Up Pressed", 0xffffff);
	}
	if (InputManager::GetInstance().IsActionPressed(MOVE_LR_ACTION))
	{
		DrawString(SCREEN_WID / 2, 100, "Move Left or Right Pressed", 0xffffff);
	}
	x += InputManager::GetInstance().GetActionValue(MOVE_LR_ACTION) * moveSpeed;
	y += InputManager::GetInstance().GetActionValue(MOVE_UD_ACTION) * moveSpeed;
	DrawBox(100 + (int)x, 100 + (int)y, 150 + (int)x, 150 + (int)y, 0xff0000, true);

}
