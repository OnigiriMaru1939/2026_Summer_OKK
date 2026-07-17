#define NOMINMAX
#include <DxLib.h>

#include "ScenePause.h"
#include "FileManager.h"
#include "Application.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Fonts.h"
#include "SceneResult.h"

ScenePause::ScenePause(FileManager& fileMng, SceneManager& sceneMng, const std::string& ip) : SceneSuper(fileMng, sceneMng),
_networkMng(sceneMng.GetNetworkManager())
{
	_textFont = fileMng_.CreateFontFM(Fonts::DotGothic16::PATH,
									  Fonts::DotGothic16::NAME,
									  60);
	_pauseFont = fileMng_.CreateFontFM(Fonts::DotGothic16::PATH,
									   Fonts::DotGothic16::NAME,
									   80);
	_explainFont = fileMng_.CreateFontFM(Fonts::DotGothic16::PATH,
									   Fonts::DotGothic16::NAME,
									   40);

	_selectedIndex = 0;
	_blockImg = fileMng_.LoadImageFM("Resource/Image/Pause/Pause_Block.png");
	_blockSelectImg = fileMng_.LoadImageFM("Resource/Image/Pause/Pause_Block_Select.png");
	_decideSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");
	_cursorSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Cursor_SE.wav");

	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_Jump.png", Application::SCREEN_WID / 4 * 3, 260.0f, false);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_Jump_Pad.png", Application::SCREEN_WID / 4 * 3, 260.0f, true);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_Rot.png", Application::SCREEN_WID / 4 * 3, 420.0f, false);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_Rot_Pad.png", Application::SCREEN_WID / 4 * 3, 420.0f, true);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_SodaAdd.png", Application::SCREEN_WID / 4 * 3, 580.0f, false);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_SodaAdd_Pad.png", Application::SCREEN_WID / 4 * 3, 580.0f, true);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_SodaJump.png", Application::SCREEN_WID / 4 * 3, 740.0f, false);
	SetTutorial("Resource/Image/Game/Tutorial/Tutorial_SodaJump_Pad.png", Application::SCREEN_WID / 4 * 3, 740.0f, true);

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
	sceneMng.SetTransitionDuration(45.0f);
}

ScenePause::~ScenePause()
{
	InputManager::GetInstance().PopLayer();
}

void ScenePause::SetTutorial(std::string path, float x, float y, bool isPad)
{
	SceneGame::tutorialPanel newTutorialPanel;
	newTutorialPanel.path = fileMng_.LoadImageFM(path);
	newTutorialPanel.x = x;
	newTutorialPanel.y = y;
	newTutorialPanel.isPad = isPad;

	_tutorialPanelList.push_back(newTutorialPanel);
}

void ScenePause::Update()
{
	sceneMng_.GetNetworkManager().ReceivePauseData(this);
}

void ScenePause::Draw()
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // 半透明に設定
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true); // 黒い半透明の四角を描画
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを元に戻す

	std::string text = "PAUSE";
	int width = GetDrawStringWidthToHandle(text.c_str(), text.size(), _pauseFont->GetHandle());
	int color = 0xaaebfb;
	DrawStringToHandle(Application::SCREEN_WID / 2 - width / 2, 70, text.c_str(), color, _pauseFont->GetHandle());

	text = "SELECT SPACE / A";
	width = GetDrawStringWidthToHandle(text.c_str(), text.size(), _explainFont->GetHandle());
	color = 0xaaebfb;
	DrawStringToHandle(Application::SCREEN_WID / 2 - width / 2, 160, text.c_str(), 0xffffff, _explainFont->GetHandle());

	for (int i = 0; i < static_cast<int>(NextScenePause::Max); i++)
	{
		i == 0 ? text = "BACK" : i == 1 ? text = "RETRY" : i == 2 ? text = "STAGE SELECT" : i == 3 ? text = "TITLE" : text = "EXIT";
		width = GetDrawStringWidthToHandle(text.c_str(), text.size(), _textFont->GetHandle());
		color = 0xaaebfb;

		if (_selectedIndex == i)
		{
			color = 0xf7c3ef;
			DrawGraph(BUTTON_X, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i, _blockSelectImg->GetHandle(), true);
			DrawStringToHandle(BUTTON_X + BUTTON_WID / 2 - width / 2, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + BUTTON_HIG / 2 - GetFontSizeToHandle(_textFont->GetHandle()) / 2, text.c_str(), color, _textFont->GetHandle());
		}
		else
		{
			DrawGraph(BUTTON_X, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i, _blockImg->GetHandle(), true);
			DrawStringToHandle(BUTTON_X + BUTTON_WID / 2 - width / 2, BUTTON_Y + (BUTTON_HIG + BUTTON_MARGIN) * i + BUTTON_HIG / 2 - GetFontSizeToHandle(_textFont->GetHandle()) / 2, text.c_str(), color, _textFont->GetHandle());
		}
	}

	for (auto& t : _tutorialPanelList)
	{
		if (t.isPad == InputManager::GetInstance().IsPadConnect())
		{
			DrawExtendGraph(static_cast<int>(t.x), static_cast<int>(t.y), static_cast<int>(t.x + t.path->GetWidth() * 1.9f), static_cast<int>(t.y + t.path->GetHeight() * 1.9f),  t.path->GetHandle(), true);
		}
	}
}

void ScenePause::SyncResume()
{
	sceneMng_.PopScene();
}

void ScenePause::SyncChangeScene(int nextSceneIndex)
{
	_selectedIndex = nextSceneIndex;
	DecidePauseScene(this);
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

void ScenePause::DecidePauseScene(bool isFromNetwork)
{
	SceneID scene = SceneID::NONE;

	if (static_cast<NextScenePause>(_selectedIndex) == NextScenePause::Back)
	{
		// 戻る（再開）処理
		if (!isFromNetwork)
		{
			SystemPacket p;
			p.type = PACKET_RESUME;
			sceneMng_.GetNetworkManager().SendSystem(p);
		}
		sceneMng_.PopScene();
		return;
	}

	switch (static_cast<NextScenePause>(_selectedIndex))
	{
		case NextScenePause::Retry:
			scene = SceneID::GAME;
			SetNextScene(scene);
			break;
		case NextScenePause::StageSelect:
			scene = SceneID::STAGE_SELECT;
			SetNextScene(scene);
			break;
		case NextScenePause::Title:
			scene = SceneID::TITLE;
			SetNextScene(scene);
			break;
		case NextScenePause::Exit:
			scene = SceneID::EXIT;
			SetNextScene(scene);
			break;
		default:
			break;
	}
	if (scene != SceneID::NONE)
	{
		// 自分が操作した時だけ、相手に教える（無限ループ防止）
		if (!isFromNetwork)
		{
			ChangeScenePacket csp;
			csp.type = PACKET_CHANGE_SCENE;
			csp.nextScene = static_cast<int>(_selectedIndex);
			sceneMng_.GetNetworkManager().SendChangeScene(csp);
		}
		SetNextScene(scene);
		isEnd = true;
	}
}
