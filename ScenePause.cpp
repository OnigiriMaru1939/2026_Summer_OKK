#define NOMINMAX
#include "ScenePause.h"
#include "FileManager.h"
#include <DxLib.h>
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"

ScenePause::ScenePause(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng), sceneMng_(sceneMng)
{
	AddFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
	_fontHandle = CreateFontToHandle("DotGothic16", 60, -1, DX_FONTTYPE_NORMAL);
	_fontPauseHandle = CreateFontToHandle("DotGothic16", 80, -1, DX_FONTTYPE_NORMAL);
	_fontExplainHandle = CreateFontToHandle("DotGothic16", 40, -1, DX_FONTTYPE_NORMAL);
	_selectedIndex = 0;
	_blockImg = fileMng_.LoadImageFM("Resource/Image/Pause/Pause_Block.png");
	_blockSelectImg = fileMng_.LoadImageFM("Resource/Image/Pause/Pause_Block_Select.png");
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
	DeleteFontToHandle(_fontHandle);
	DeleteFontToHandle(_fontPauseHandle);
	RemoveFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
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

	std::string text = "PAUSE";
	int width = GetDrawStringWidthToHandle(text.c_str(), text.size(), _fontPauseHandle);
	int color = 0xaaebfb;
	DrawStringToHandle(Application::SCREEN_WID / 2 - width / 2, 70, text.c_str(), color, _fontPauseHandle);

	text = "SELECT SPACE / A";
	width = GetDrawStringWidthToHandle(text.c_str(), text.size(), _fontExplainHandle);
	color = 0xaaebfb;
	DrawStringToHandle(Application::SCREEN_WID / 2 - width / 2, 160, text.c_str(), 0xffffff, _fontExplainHandle);

	for (int i = 0; i < static_cast<int>(NextScenePause::Max); i++)
	{
		i == 0 ? text = "BACK" : i == 1 ? text = "RETRY" : i == 2 ? text = "STAGE SELECT" : i == 3 ? text = "TITLE" : text = "EXIT";
		width = GetDrawStringWidthToHandle(text.c_str(), text.size(), _fontHandle);
		color = 0xaaebfb;

		if (_selectedIndex == i)
		{
			color = 0xf7c3ef;
			DrawGraph(BUTTON_X, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i, _blockSelectImg->GetHandle(), true);
			DrawStringToHandle(BUTTON_X + BUTTON_WID / 2 - width / 2, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + BUTTON_HIG / 2 - GetFontSizeToHandle(_fontHandle) / 2, text.c_str(), color, _fontHandle);
		}
		else
		{
			DrawGraph(BUTTON_X, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i, _blockImg->GetHandle(), true);
			DrawStringToHandle(BUTTON_X + BUTTON_WID / 2 - width / 2, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + BUTTON_HIG / 2 - GetFontSizeToHandle(_fontHandle) / 2, text.c_str(), color, _fontHandle);
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
