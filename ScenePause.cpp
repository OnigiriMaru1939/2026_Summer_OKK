#define NOMINMAX
#include "ScenePause.h"
#include "FileManager.h"
#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"

ScenePause::ScenePause(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng), sceneMng_(sceneMng)
{
	_selectedIndex = 0;
	_decideSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");
	_cursorSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Cursor_SE.wav");

	InputManager::GetInstance().PushLayer(); // 新しい入力レイヤーを追加
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   sceneMng_.PopScene();
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Pause,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   sceneMng_.PopScene();
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveV,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveV);
														   MoveSelect(moveValue);
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   _decideSE->PlayOneShot();
														   DecidePauseScene();
													   }
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

	DrawString(500, 100, "SELECT SPACE / A", 0xffffff);
	DrawFormatString(500, 0, 0x0000ff, "SelectedIndex = %d", _selectedIndex);


	for (int i = 0; i < static_cast<int>(NextScenePause::Max); i++)
	{
		std::string text = "";
		i == 0 ? text = "BACK" : i == 1 ? text = "RETRY" : i == 2 ? text = "STAGE SELECT" : i == 3 ? text = "TITLE" : text = "EXIT";
		DrawBox(BUTTON_X, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i, BUTTON_X + BUTTON_WID, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + BUTTON_HIG, 0x11333333 * i, true);
		DrawString(BUTTON_X + 20, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + 20, text.c_str(), 0x00ff00);
		if (_selectedIndex == i)
		{
			DrawBox(BUTTON_X - 5, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i - 5, BUTTON_X + BUTTON_WID + 5, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + BUTTON_HIG + 5, 0xff0000, false);
		}
	}
}

void ScenePause::MoveSelect(float moveValue)
{
	if (moveValue > 0)
	{
		if (_selectedIndex < static_cast<int>(NextScenePause::Max))
		{
			_selectedIndex++;
			_cursorSE->PlayOneShot();
		}
	}
	else
	{
		if (_selectedIndex > 0)
		{
			_selectedIndex--;
			_cursorSE->PlayOneShot();
		}
	}
	_selectedIndex = std::max(0, std::min(static_cast<int>(NextScenePause::Max) - 1, _selectedIndex));
}

void ScenePause::DecidePauseScene()
{
	switch (static_cast<NextScenePause>(_selectedIndex))
	{
		case NextScenePause::Back:
			sceneMng_.PopScene();
			break;
		case NextScenePause::Retry:
			SetNextScene(SceneID::GAME);
			break;
		case NextScenePause::StageSelect:
			SetNextScene(SceneID::STAGE_SELECT);
			break;
		case NextScenePause::Title:
			SetNextScene(SceneID::TITLE);
			break;
		case NextScenePause::Exit:
			SetNextScene(SceneID::EXIT);
			break;
		default:
			break;
	}
	isEnd = true;
}
