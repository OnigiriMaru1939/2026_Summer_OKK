#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "FileManager.h"
#include "Fps.h"

//constexpr auto DEBUG_PARTICLE_PATH = "Resource/ParticleJsonData/parametera.json";

// デバッグ---------------------
//#include "ParticleManager.h"
// ----------------------------

Application::Application()
{
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	SetWindowText("サイダーパンク");
	SetWindowSizeChangeEnableFlag(TRUE, TRUE);
	SetDoubleStartValidFlag(TRUE);
	SetAlwaysRunFlag(TRUE);

	SetGraphMode(SCREEN_WID - 100, SCREEN_HIG - 100, 32);
	ChangeWindowMode(true);
#ifdef _DEBUG
	SetGraphMode(SCREEN_WID, SCREEN_HIG, 32);
	ChangeWindowMode(false);
#endif // DEBUG

//#ifdef _WIN64
//	Live2D_SetCubism4CoreDLLPath(TEXT("CubismSdkForNative-5-r.4.1/Core/dll/windows/x86_64/Live2DCubismCore.dll"));
//#else
//	Live2D_SetCubism4CoreDLLPath(TEXT("CubismSdkForNative-5-r.4.1/Core/dll/windows/x86/Live2DCubismCore.dll"));
//#endif

	DxLib_Init();
	fileMng = std::make_unique<FileManager>();
	sceneMng = std::make_unique<SceneManager>(*fileMng);
	// デバッグ----------------------------------------------------
	//pMng = std::make_unique<ParticleManager>(*fileMng);
	//pMng->RegisterConfig(DEBUG_PARTICLE_PATH);
	//------------------------------------------------------------
}

Application::~Application()
{
	// デストラクト明示
	sceneMng.reset();
	fileMng.reset();
	InputManager::GetInstance().ClearAxisCallbacks();
	InputManager::GetInstance().ClearPressCallbacks();
	InputManager::GetInstance().ClearTriggerCallbacks();
	InputManager::GetInstance().ClearReleaseCallbacks();
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
	sceneMng->Update();
}

void Application::Draw()
{
	sceneMng->Draw();

	InputManager::GetInstance().DrawDebug(0, 40);
}

void Application::DebugMoveX()
{
	x += InputManager::GetInstance().GetActionValue(ActionID::MoveH) * moveSpeed;
	x += InputManager::GetInstance().GetActionAxis(ActionID::MoveHAxis) * moveSpeed;
}

void Application::DebugMoveY()
{
	y -= InputManager::GetInstance().GetActionValue(ActionID::MoveV) * moveSpeed;
}

void Application::DeBugJump()
{
	color == 0xffffff ? color = 0xff0000 : color = 0xffffff;
}