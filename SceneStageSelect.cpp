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
	titleFontHandle = CreateFontToHandle("DotGothic16", 100, -1, DX_FONTTYPE_NORMAL);
	_selectedIndex = 1; // 初期選択ステージは1

	_bgImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_bg.png");
	_stageSelectH1Img = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_H1.png");
	_stageSelectBlockImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_Block.png");
	_stageSelectBlockSelectedImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_Selected_Block.png");

	_decideSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");
	_cursorSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Cursor_SE.wav");

	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveH,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   // 2*2のブロック配置のため、1と2、3と4で選択が切り替わる
														   float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveH);
														   MoveSelect(moveValue, true);
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveV,
												   [this]()
												   {
													   if (!isTransition)
													   {
															// 2*2のブロック配置のため、1と3、2と4で選択が切り替わる
															float moveValue = InputManager::GetInstance().GetActionValue(ActionID::MoveV);
															MoveSelect(moveValue, false);
													   }
												   });

	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   _decideSE->PlayOneShot();
														   if (_selectedIndex == 0)
														   {
															   // タイトルが選択されている場合はタイトルに戻る
															   SetNextScene(SceneID::TITLE);
															   isEnd = true;
															   return;
														   }
														   // 選択されたステージをSceneGameに渡すために、SceneManagerに保存
														   SceneGame::SetSelectedStageIndex(_selectedIndex);
														   SetNextScene(SceneID::GAME);
														   isEnd = true;
													   }
												   });
}

SceneStageSelect::~SceneStageSelect()
{
	DeleteFontToHandle(stageFontHandle);
	DeleteFontToHandle(stageNumFontHandle);
	DeleteFontToHandle(titleFontHandle);
	RemoveFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
	//RemoveFontResourceExA("Resource/fonts/KumarOneOutline-Regular.ttf", FR_PRIVATE, NULL);
}

void SceneStageSelect::Update()
{

}

void SceneStageSelect::Draw()
{
	DrawGraph(0, 0, _bgImg->GetHandle(), true);
	DrawGraph(H1_X, H1_Y, _stageSelectH1Img->GetHandle(), true);
	// 4つを一グループで描画
	for (int i = 0; i < BLOCK_NUM; ++i)
	{
		int x = BLOCK_X + (i % BLOCK_NUM_ROW) * (_stageSelectBlockImg->GetWidth() + BLOCK_SPACING_X);
		int y = BLOCK_Y + (i / BLOCK_NUM_ROW) * (_stageSelectBlockImg->GetHeight() + BLOCK_SPACING_Y);

		std::string stageText = "STAGE";
		int width = GetDrawStringWidthToHandle(stageText.c_str(), stageText.length(), stageFontHandle);
		int color = 0xaaebfb;
		if (_selectedIndex == i + 1)
		{
			DrawGraph(x, y, _stageSelectBlockSelectedImg->GetHandle(), true);
			color = 0xf7c3ef;
			DrawStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (width / 2), y + 40, stageText.c_str(), color, stageFontHandle);
		}
		else
		{
			DrawGraph(x, y, _stageSelectBlockImg->GetHandle(), true);
			DrawStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (width / 2), y + 40, stageText.c_str(), color, stageFontHandle);
		}
		
		std::string stageNumStr = std::to_string(i + 1);
		int numWidth = GetDrawStringWidthToHandle(stageNumStr.c_str(), stageNumStr.length(), stageNumFontHandle);
		DrawFormatStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (numWidth / 2), y + 140, 0xffffff, stageNumFontHandle, "%d", i + 1);

	}
	DrawBox(TITLE_X, TITLE_Y, TITLE_X + TITLE_W, TITLE_Y + TITLE_H, GetColor(255, 0, 255), true);
	std::string titleText = "TITLE";
	int titleWidth = GetDrawStringWidthToHandle(titleText.c_str(), titleText.length(), titleFontHandle);
	int titleHeight = GetFontSizeToHandle(titleFontHandle);
	DrawStringToHandle(TITLE_X + (TITLE_W / 2) - (titleWidth / 2), TITLE_Y + (TITLE_H / 2) - (titleHeight / 2), titleText.c_str(), GetColor(255, 255, 255), titleFontHandle);
	if (_selectedIndex == 0)
	{
		DrawBox(TITLE_X - 5, TITLE_Y - 5, TITLE_X + TITLE_W + 5, TITLE_Y + TITLE_H + 5, GetColor(255, 0, 0), false);
	}
}

void SceneStageSelect::MoveSelect(float moveValue, bool isHorizontal)
{
	if (isHorizontal)
	{
		if (moveValue > 0)
		{
			// 右移動
			if (_selectedIndex == 0)
			{
				_selectedIndex = 3; // タイトルボタンから右に移動した場合は左下のステージに移動
				_cursorSE->PlayOneShot();
			}
			else if (_selectedIndex % 2 != 0)
			{
				_selectedIndex++;
				_cursorSE->PlayOneShot();		
			}
		}
		else
		{
			// 左移動
			// 左下のステージから左に移動した場合はタイトルに移動、それ以外は通常の左移動
			if (_selectedIndex == 3)
			{
				_selectedIndex = 0; // タイトルに移動
				_cursorSE->PlayOneShot();
			}
			else if (_selectedIndex % 2 == 0 && _selectedIndex != 0)
			{
				_selectedIndex--;
				_cursorSE->PlayOneShot();
			}
		}
	}
	else
	{
		if (moveValue > 0)
		{
			// 下移動
			if (0 < _selectedIndex && _selectedIndex <= 2)
			{
				_selectedIndex += 2;
				_cursorSE->PlayOneShot();
			}
		}
		else
		{
			// 上移動
			if (_selectedIndex > 2)
			{
				_selectedIndex -= 2;
				_cursorSE->PlayOneShot();
			}
		}
	}
	// ステージは0～4の範囲でループ
	_selectedIndex = std::max(0, std::min(4, _selectedIndex));
}