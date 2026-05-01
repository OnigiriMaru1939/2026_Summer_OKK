#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "FileManager.h"
#include "Fps.h"

constexpr auto DEBUG_PARTICLE_PATH = "resources/particleJsonData/waterParameter.json";

// デバッグ---------------------
#include "ParticleManager.h"
// ----------------------------

Application::Application()
{
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	SetWindowText("サイダーパンク");
	SetGraphMode(SCREEN_WID, SCREEN_HIG, 32);
	ChangeWindowMode(false);

//#ifdef _WIN64
//	Live2D_SetCubism4CoreDLLPath(TEXT("CubismSdkForNative-5-r.4.1/Core/dll/windows/x86_64/Live2DCubismCore.dll"));
//#else
//	Live2D_SetCubism4CoreDLLPath(TEXT("CubismSdkForNative-5-r.4.1/Core/dll/windows/x86/Live2DCubismCore.dll"));
//#endif

	DxLib_Init();
	//SetMouseDispFlag(true);
	fileMng = std::make_unique<FileManager>();
	sceneMng = std::make_unique<SceneManager>(*fileMng);
	// デバッグ----------------------------------------------------
	pMng = std::make_unique<ParticleManager>(*fileMng);
	pMng->RegisterConfig(DEBUG_PARTICLE_PATH);
	//------------------------------------------------------------

	// イベントの登録
	InputManager::GetInstance().SetPressCallback(ActionID::MoveH, [this]() { DebugMoveX(); });
	InputManager::GetInstance().SetPressCallback(ActionID::MoveV, [this]() { DebugMoveY(); });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Jump, [this]() { DeBugJump(); });
}

Application::~Application()
{
	// デストラクト明示
	sceneMng.reset();
	fileMng.reset();
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
	InputManager::GetInstance().Update();
	sceneMng->Update();

	// デバッグ
	if (InputManager::GetInstance().IsMouseTriggered(MouseButton::Mouse_Left))
	{
		int mx, my;
		GetMousePoint(&mx, &my);
		pMng->PlayParticle(DEBUG_PARTICLE_PATH, (float)mx, (float)my);
	}
	pMng->UpdateAll();
}

void Application::Draw()
{
	sceneMng->Draw();

	InputManager::GetInstance().DrawDebug(0, 40);
	DrawBox(100 + (int)x, 100 + (int)y, 150 + (int)x, 150 + (int)y, color, true);

	pMng->DrawAll();

}

void Application::DebugMoveX()
{
	x += InputManager::GetInstance().GetActionValue(ActionID::MoveH) * moveSpeed;
}

void Application::DebugMoveY()
{
	y += InputManager::GetInstance().GetActionValue(ActionID::MoveV) * moveSpeed;
}

void Application::DeBugJump()
{
	color == 0xffffff ? color = 0xff0000 : color = 0xffffff;
}