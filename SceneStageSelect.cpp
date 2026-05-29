#define NOMINMAX
#include <algorithm>
#include <string>
#include "SceneStageSelect.h"
#include "InputManager.h"
#include "SceneGame.h"

SceneStageSelect::SceneStageSelect(FileManager& fileMng) : SceneSuper(fileMng)
{
	//AddFontResourceExA("Resource/fonts/KumarOneOutline-Regular.ttf", FR_PRIVATE, NULL);
	//stageFontHandle = CreateFontToHandle("Kumar One Outline", 60, -1, DX_FONTTYPE_NORMAL);
	AddFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
	stageFontHandle = CreateFontToHandle("DotGothic16", 80, -1, DX_FONTTYPE_NORMAL);
	stageNumFontHandle = CreateFontToHandle("DotGothic16", 120, -1, DX_FONTTYPE_NORMAL);
	_selectedStage = 1; // 初期選択ステージは1

	_bgImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_bg.png");
	_stageSelectH1Img = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_H1.png");
	_stageSelectBlockImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_Block.png");

	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveH,
												   [this]()
												   {
														// 2*2のブロック配置のため、1と2、3と4で選択が切り替わる
														float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveH);
														MoveSelect(moveValue, true);
													});
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveV,
												   [this]()
												   {
														// 2*2のブロック配置のため、1と3、2と4で選択が切り替わる
														float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveV);
														MoveSelect(moveValue, false);
												   });

	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   // 選択されたステージをSceneGameに渡すために、SceneManagerに保存
													   SceneGame::SetSelectedStageIndex(_selectedStage);
														SetNextScene(SceneID::GAME);
														isEnd = true;
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel,
												   [this]()
												   {
													   SetNextScene(SceneID::TITLE);
													   isEnd = true;
												   });
}

SceneStageSelect::~SceneStageSelect()
{
	DeleteFontToHandle(stageFontHandle);
	DeleteFontToHandle(stageNumFontHandle);
	//RemoveFontResourceExA("Resource/fonts/KumarOneOutline-Regular.ttf", FR_PRIVATE, NULL);
	RemoveFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
}

void SceneStageSelect::Update()
{

}

void SceneStageSelect::Draw()
{
	DrawGraph(0, 0, _bgImg->GetHandle(), true);
	DrawGraph(H1_X, H1_Y, _stageSelectH1Img->GetHandle(), true);
	// 4つを一グループで描画
	for (int i = 0; i < 4; ++i)
	{
		int x = BLOCK_X + (i % 2) * (_stageSelectBlockImg->GetWidth() + 20);
		int y = BLOCK_Y + (i / 2) * (_stageSelectBlockImg->GetHeight() + 20);
		DrawGraph(x, y, _stageSelectBlockImg->GetHandle(), true);

		std::string stageText = "STAGE";
		int width = GetDrawStringWidthToHandle(stageText.c_str(), stageText.length(), stageFontHandle);
		DrawStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (width / 2), y + 40, stageText.c_str(), 0xaaebfb, stageFontHandle);
		
		std::string stageNumStr = std::to_string(i + 1);
		int numWidth = GetDrawStringWidthToHandle(stageNumStr.c_str(), stageNumStr.length(), stageNumFontHandle);
		DrawFormatStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (numWidth / 2), y + 140, 0xffffff, stageNumFontHandle, "%d", i + 1);
		if (_selectedStage == i + 1)
		{
			// 選択中のステージに赤い枠を描画
			DrawBox(x - 5, y - 5, x + _stageSelectBlockImg->GetWidth() + 5, y + _stageSelectBlockImg->GetHeight() + 5, GetColor(255, 0, 0), false);
		}
	}
	DrawString(0, 20, "STAGESELECT SCENE", 0xffffff);
}

void SceneStageSelect::MoveSelect(float moveValue, bool isHorizontal)
{
	if (isHorizontal)
	{
		if (moveValue > 0)
		{
			// 右移動
			_selectedStage = (_selectedStage % 2 == 0) ? _selectedStage : _selectedStage + 1;
		}
		else
		{
			// 左移動
			_selectedStage = (_selectedStage % 2 == 0) ? _selectedStage - 1 : _selectedStage;
		}
	}
	else
	{
		if (moveValue > 0)
		{
			// 下移動
			_selectedStage = (_selectedStage <= 2) ? _selectedStage + 2 : _selectedStage;
		}
		else
		{
			// 上移動
			_selectedStage = (_selectedStage > 2) ? _selectedStage - 2 : _selectedStage;
		}
	}
	// ステージは1～4の範囲でループ
	_selectedStage = std::max(1, std::min(4, _selectedStage));
}
