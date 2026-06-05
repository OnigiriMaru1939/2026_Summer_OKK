#include "ScenePause.h"
#include "FileManager.h"
#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"

ScenePause::ScenePause(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng), sceneMng_(sceneMng)
{
	InputManager::GetInstance().PushLayer(); // 新しい入力レイヤーを追加
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel,
												   [this]()
												   {
													   SetNextScene(SceneID::EXIT);
													   isEnd = true;
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Pause,
												   [this]()
												   {
													   sceneMng_.PopScene();
												   });
}

ScenePause::~ScenePause()
{
	InputManager::GetInstance().PopLayer();
}

void ScenePause::Update()
{
}

void ScenePause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // 半透明に設定
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true); // 黒い半透明の四角を描画
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを元に戻す

	DrawString(100, 100, "PAUSE \n(Press Escape/Back to Exit, Press Tab/Start to Resume)", 0xffffff);
}
