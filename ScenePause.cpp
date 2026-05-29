#include "ScenePause.h"
#include "FileManager.h"
#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"

ScenePause::ScenePause(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng), sceneMng_(sceneMng)
{
	InputManager::GetInstance().SetPauseMode(true);
}

ScenePause::~ScenePause()
{
	InputManager::GetInstance().SetPauseMode(false);
}

void ScenePause::Update()
{
}

void ScenePause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // 半透明に設定
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true); // 黒い半透明の四角を描画
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを元に戻す
}
