#include "SceneResult.h"
#include "SceneGame.h"
#include "InputManager.h"
#include <string>

SceneResult::SceneResult(FileManager& fileMng, bool isClear, ClearResult& result) : SceneSuper(fileMng), _isClear(isClear), _clearResult(result)
{
	SceneGame::selectedStageIndex_ == 4 ? _selectedNext = 1 : _selectedNext = 0;

	AddFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
	resultFontHandle = CreateFontToHandle("DotGothic16", 80, -1, DX_FONTTYPE_NORMAL);
	nextButtonFontHandle = CreateFontToHandle("DotGothic16", 40, -1, DX_FONTTYPE_NORMAL);

	_bgImg = fileMng.LoadImageFM("Resource/Image/Title_bg_NoLogo.png");
	_resultClearLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Clear_Logo.png");
	_resultFailedLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Failed_Logo.png");
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   switch (static_cast<NextScene>(_selectedNext))
													   {
														   case NextScene::Next:
															   SceneGame::selectedStageIndex_++;
															   SetNextScene(SceneID::GAME);
															   break;
														   case NextScene::StageSelect:
															   SetNextScene(SceneID::STAGE_SELECT);
															   break;
														   case NextScene::Title:
															   SetNextScene(SceneID::TITLE);
															   break;
													   }
													   
													   isEnd = true;
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveH,
												   [this]()
												   {
													   float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveH);
													   MoveSelect(moveValue);
												   });
}

SceneResult::~SceneResult()
{
	DeleteFontToHandle(resultFontHandle);
	RemoveFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
}

void SceneResult::Update()
{

}

void SceneResult::Draw()
{
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, _bgImg->GetHandle(), true);
	if (_isClear)
	{
		DrawRotaGraph(LOGO_X, LOGO_Y, 1.0f, -5.0f * (DX_PI_F / 180.0f), _resultClearLogoImg->GetHandle(), true);
		DrawFormatStringToHandle(RESULT_TIME_X, RESULT_TIME_Y, 0xffffff, resultFontHandle, "CLEAR TIME: %.2f", _clearResult.time);
	}
	else
	{
		DrawRotaGraph(LOGO_X, LOGO_Y, 1.0f, -5.0f * (DX_PI_F / 180.0f), _resultFailedLogoImg->GetHandle(), true);
	}

	for (int i = 0; i < static_cast<int>(NextScene::Max); i++)
	{
		if (i == 0)
		{
			if (SceneGame::selectedStageIndex_ == 4) continue;
		}
		std::string text = "";
		i == 0 ? text = "NEXT STAGE" : i == 1 ? text = "STAGE SELECT" : text = "TITLE";
		int textWidth = GetDrawStringWidthToHandle(text.c_str(), static_cast<int>(strlen(text.c_str())), nextButtonFontHandle);
		int x = NEXT_BUTTON_X + (NEXT_BUTTON_MARGIN + NEXT_BUTTON_WID) * i;
		DrawBox(x, NEXT_BUTTON_Y, x + NEXT_BUTTON_WID, NEXT_BUTTON_Y + NEXT_BUTTON_HIG, 0x114444 + 0x114444 * i, true);
		DrawStringToHandle((x + NEXT_BUTTON_WID / 2) - (textWidth / 2), (NEXT_BUTTON_Y + NEXT_BUTTON_HIG / 2) - (GetFontSizeToHandle(nextButtonFontHandle) / 2), text.c_str(), 0xffffff, nextButtonFontHandle);
		if (_selectedNext == i)
		{
			DrawBox(x - 5, NEXT_BUTTON_Y - 5, x + NEXT_BUTTON_WID + 5, NEXT_BUTTON_Y + NEXT_BUTTON_HIG + 5, 0xff0000, false);
		}
	}
}

void SceneResult::MoveSelect(float moveValue)
{
	if (moveValue > 0)
	{
		_selectedNext = (_selectedNext == 2) ? _selectedNext : _selectedNext + 1;
	}
	else
	{
		_selectedNext = (SceneGame::selectedStageIndex_ == 4) ? (_selectedNext == 1) ? _selectedNext : _selectedNext - 1 : (_selectedNext == 0) ? _selectedNext : _selectedNext - 1;
	}
}
