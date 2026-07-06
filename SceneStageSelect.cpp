#define NOMINMAX
#include <algorithm>
#include <string>
#include "SceneStageSelect.h"
#include "InputManager.h"
#include "SceneGame.h"
#include "Application.h"
#include "Fonts.h"

SceneStageSelect::SceneStageSelect(FileManager& fileMng, SceneManager& sceneMng, const std::string& ip) : SceneSuper(fileMng, sceneMng),
_networkMng(sceneMng.GetNetworkManager())
{
	_fadeAlpha = 255.0f;
	// 仮別フォント
	//AddFontResourceExA("Resource/fonts/KumarOneOutline-Regular.ttf", FR_PRIVATE, NULL);
	//stageFontHandle = CreateFontToHandle("Kumar One Outline", 60, -1, DX_FONTTYPE_NORMAL);
	_stageFont = fileMng.CreateFontFM(Fonts::DotGothic16::PATH,
									  Fonts::DotGothic16::NAME,
									  80);
	_stageNumFont = fileMng.CreateFontFM(Fonts::DotGothic16::PATH,
									  Fonts::DotGothic16::NAME,
									  120);
	_titleFont = fileMng.CreateFontFM(Fonts::DotGothic16::PATH,
									  Fonts::DotGothic16::NAME,
									  100);
	_selectedIndex = 1; // 初期選択ステージは1

	_bgImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_bg.png");
	_stageSelectH1Img = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_H1.png");
	_stageSelectBlockImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_Block.png");
	_stageSelectBlockSelectedImg = fileMng.LoadImageFM("Resource/Image/StageSelect/StageSelect_Selected_Block.png");

	_decideSE = fileMng.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");
	_cursorSE = fileMng.LoadSoundFM("Resource/Sound/SE/Cursor_SE.wav");

	// ネットワーク初期化
	//bool isHost = sceneMng.GetIsHost();
	//_networkMng.Initialize(isHost, ip);

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
														   _networkMng.SendStageSelect(_selectedIndex);
														   SceneGame::SetSelectedStageIndex(_selectedIndex);
														   SetNextScene(SceneID::GAME);
														   isEnd = true;
													   }
												   });

	sceneMng.SetTransitionDuration(45.0f);
}

SceneStageSelect::~SceneStageSelect()
{

}

void SceneStageSelect::Update()
{
	int receivedStageIndex = -1;
	if (!isTransition && !isEnd && _networkMng.ReceiveStageSelect(receivedStageIndex))
	{
		// 有効なステージ番号（0より大きい）を受け取ったら強制遷移
		if (receivedStageIndex > 0)
		{
			_selectedIndex = receivedStageIndex;
			_decideSE->PlayOneShot(); // 相手が決定したときもSEを鳴らしてあげる
			SceneGame::SetSelectedStageIndex(receivedStageIndex);
			SetNextScene(SceneID::GAME);
			isEnd = true;
		}
	}
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
		int width = GetDrawStringWidthToHandle(stageText.c_str(), stageText.length(), _stageFont->GetHandle());
		int color = 0xaaebfb;
		if (_selectedIndex == i + 1)
		{
			DrawGraph(x, y, _stageSelectBlockSelectedImg->GetHandle(), true);
			color = 0xf7c3ef;
			DrawStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (width / 2), y + 40, stageText.c_str(), color, _stageFont->GetHandle());
		}
		else
		{
			DrawGraph(x, y, _stageSelectBlockImg->GetHandle(), true);
			DrawStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (width / 2), y + 40, stageText.c_str(), color, _stageFont->GetHandle());
		}
		
		std::string stageNumStr = std::to_string(i + 1);
		int numWidth = GetDrawStringWidthToHandle(stageNumStr.c_str(), stageNumStr.length(), _stageNumFont->GetHandle());
		DrawFormatStringToHandle(x + (_stageSelectBlockImg->GetWidth() / 2) - (numWidth / 2), y + 140, 0xffffff, _stageNumFont->GetHandle(), "%d", i + 1);

	}
	DrawBox(TITLE_X, TITLE_Y, TITLE_X + TITLE_W, TITLE_Y + TITLE_H, GetColor(255, 0, 255), true);
	std::string titleText = "TITLE";
	int titleWidth = GetDrawStringWidthToHandle(titleText.c_str(), titleText.length(), _titleFont->GetHandle());
	int titleHeight = GetFontSizeToHandle(_titleFont->GetHandle());
	DrawStringToHandle(TITLE_X + (TITLE_W / 2) - (titleWidth / 2), TITLE_Y + (TITLE_H / 2) - (titleHeight / 2), titleText.c_str(), GetColor(255, 255, 255), _titleFont->GetHandle());
	if (_selectedIndex == 0)
	{
		DrawBox(TITLE_X - 5, TITLE_Y - 5, TITLE_X + TITLE_W + 5, TITLE_Y + TITLE_H + 5, GetColor(255, 0, 0), false);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeAlpha));
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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

void SceneStageSelect::TransitionIn(float t)
{
	float e = EaseInCubic(1 - t);
	_fadeAlpha = e * 255.0f;
}

void SceneStageSelect::TransitionOut(float t)
{
	_fadeAlpha = EaseOutCubic(t) * 255.0f;
}
