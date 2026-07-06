#define NOMINMAX
#include "SceneResult.h"
#include "SceneGame.h"
#include "InputManager.h"
#include "Fonts.h"
#include <string>
#include <algorithm>

SceneResult::SceneResult(FileManager& fileMng, bool isClear, ClearResult& result, SceneManager& sceneMng, const std::string& ip) : SceneSuper(fileMng, sceneMng), _isClear(isClear), _clearResult(result),
_networkMng(sceneMng.GetNetworkManager())
{
	SceneGame::selectedStageIndex_ == 4 ? _selectedNext = 1 : _selectedNext = 0;
	_resultFont = fileMng_.CreateFontFM(Fonts::DotGothic16::PATH,
									  Fonts::DotGothic16::NAME,
									  80);
	_nextButtonFont = fileMng_.CreateFontFM(Fonts::DotGothic16::PATH,
										Fonts::DotGothic16::NAME,
										60);

	_bgImg = fileMng.LoadImageFM("Resource/Image/Result/Result_bg.png");
	_resultClearLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Clear_Logo.png");
	_resultFailedLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Failed_Logo.png");

	_decideSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");
	_cursorSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Cursor_SE.wav");

	_fadeInAlpha = 255.0f;
	_fadeOutAlpha = 0.0f;

	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   _decideSE->PlayOneShot();
														   SceneID scene = SceneID::NONE;
														   switch (static_cast<NextScene>(_selectedNext))
														   {
															   case NextScene::Next:
																   SceneGame::selectedStageIndex_++;
																   scene = SceneID::GAME;
																   SetNextScene(scene);
																   break;
															   case NextScene::Retry:
																   scene = SceneID::GAME;
																   SetNextScene(scene);
																   break;
															   case NextScene::StageSelect:
																   scene = SceneID::STAGE_SELECT;
																   SetNextScene(scene);
																   break;
															   case NextScene::Title:
																   scene = SceneID::TITLE;
																   SetNextScene(scene);
																   break;
														   }
														   if (scene != SceneID::NONE)
														   {
															   ChangeScenePacket csp;
															   csp.type = PACKET_CHANGE_SCENE;
															   csp.nextScene = static_cast<int>(_selectedNext);
															   _networkMng.SendChangeScene(csp);
														   }
														   isEnd = true;
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveH,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveH);
														   MoveSelect(moveValue);
													   }
												   });

	sceneMng.SetTransitionDuration(45.0f);
}

SceneResult::~SceneResult()
{

}

void SceneResult::Update()
{
	_networkMng.ReceiveData();
	int nextScene;
	if (_networkMng.ReceiveChangeScene(nextScene))
	{
		_decideSE->PlayOneShot();
		SceneID scene = SceneID::NONE;
		switch (static_cast<NextScene>(nextScene))
		{
			case NextScene::Next:
				SceneGame::selectedStageIndex_++;
				scene = SceneID::GAME;
				SetNextScene(scene);
				break;
			case NextScene::Retry:
				scene = SceneID::GAME;
				SetNextScene(scene);
				break;
			case NextScene::StageSelect:
				scene = SceneID::STAGE_SELECT;
				SetNextScene(scene);
				break;
			case NextScene::Title:
				scene = SceneID::TITLE;
				SetNextScene(scene);
				break;
			default:
				return;
				break;
		}
		isEnd = true;
		return;
	}
}

void SceneResult::Draw()
{
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, _bgImg->GetHandle(), true);
	if (_isClear)
	{
		DrawRotaGraph(LOGO_X, LOGO_Y, 1.0f, -5.0f * (DX_PI_F / 180.0f), _resultClearLogoImg->GetHandle(), true);
		DrawFormatStringToHandle(RESULT_TIME_X, RESULT_TIME_Y, 0xffffff, _resultFont->GetHandle(), "CLEAR TIME: %.2f", _clearResult.time);
	}
	else
	{
		DrawRotaGraph(LOGO_X, LOGO_Y, 1.0f, -5.0f * (DX_PI_F / 180.0f), _resultFailedLogoImg->GetHandle(), true);
	}

	for (int i = 0; i < static_cast<int>(NextScene::Max); i++)
	{
		if (i == static_cast<int>(NextScene::Next))
		{
			if (SceneGame::selectedStageIndex_ == 4) continue;
		}
		std::string text = "";
		i == 0 ? text = "NEXT STAGE" : i == 1 ? text = "RETRY" : i == 2 ? text = "STAGE SELECT" : text = "TITLE";

		int textWidth = GetDrawStringWidthToHandle(text.c_str(), static_cast<int>(strlen(text.c_str())), _nextButtonFont->GetHandle());
		int x = NEXT_BUTTON_X + (NEXT_BUTTON_MARGIN + NEXT_BUTTON_WID) * i;
		DrawBox(x, NEXT_BUTTON_Y, x + NEXT_BUTTON_WID, NEXT_BUTTON_Y + NEXT_BUTTON_HIG, 0x114444 * i, true);
		DrawStringToHandle((x + NEXT_BUTTON_WID / 2) - (textWidth / 2), (NEXT_BUTTON_Y + NEXT_BUTTON_HIG / 2) - (GetFontSizeToHandle(_nextButtonFont->GetHandle()) / 2), text.c_str(), 0xffffff, _nextButtonFont->GetHandle());
		if (_selectedNext == i)
		{
			DrawBox(x - 5, NEXT_BUTTON_Y - 5, x + NEXT_BUTTON_WID + 5, NEXT_BUTTON_Y + NEXT_BUTTON_HIG + 5, 0xff0000, false);
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeInAlpha));
	if (_isClear)
	{
		DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0xffffff, true);
	}
	else
	{
		DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeOutAlpha));
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneResult::MoveSelect(float moveValue)
{
	if (moveValue > 0)
	{
		if (_selectedNext < static_cast<int>(NextScene::Max))
		{
			_selectedNext++;
			_cursorSE->PlayOneShot();
		}
	}
	else
	{
		if (SceneGame::selectedStageIndex_ == 4)
		{
			if (_selectedNext > 1)
			{
				_selectedNext--;
				_cursorSE->PlayOneShot();
			}
		}
		else
		{
			if (_selectedNext > 0)
			{
				_selectedNext--;
				_cursorSE->PlayOneShot();
			}
		}
	}
	_selectedNext = std::max(0, std::min(static_cast<int>(NextScene::Max) - 1, _selectedNext));
}

void SceneResult::TransitionIn(float t)
{
		float e = EaseInCubic(1 - t);
		_fadeInAlpha = e * 255.0f;
}

void SceneResult::TransitionOut(float t)
{
	_fadeOutAlpha = EaseOutCubic(t) * 255.0f;
}
