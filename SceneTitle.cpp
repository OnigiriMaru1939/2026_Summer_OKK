#include "SceneTitle.h"
#include "InputManager.h"
#include "Application.h"
#include "BGMManager.h"

SceneTitle::SceneTitle(FileManager& fileMng, SceneManager& sceneMng) : SceneSuper(fileMng, sceneMng)
{
	_fadeAlpha = 255.0f;
	_bgImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_bg.png");
	_TitleLogoImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_Logo.png");
	_TitleStartImg = fileMng_.LoadImageFM("Resource/Image/Title/Title_Start.png");
	_mainBgm = fileMng_.LoadSoundFM("Resource/Sound/BGM/Night_Light.wav");
	_decideSE = fileMng_.LoadSoundFM("Resource/Sound/SE/Decide_SE.wav");


	BGMManager::GetInstance().PlayBGM(_mainBgm);
	_mainBgm->SetVolume(196);
	InputManager::GetInstance().SetTriggerCallback(ActionID::Cancel,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   SetNextScene(SceneID::EXIT);
														   isEnd = true;
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   if (!isTransition)
													   {
														   _decideSE->PlayOneShot();
														   SetNextScene(
															   SceneID::STAGE_SELECT);
														   if (_selectedIndex == 0)
														   {
															   sceneMng_.SetIsHost(true);  // 1P (Host)
														   }
														   else
														   {
															   sceneMng_.SetIsHost(false); // 2P (Client)
														   }
														   isEnd = true;
													   }
												   });
	InputManager::GetInstance().SetTriggerCallback(ActionID::MoveV,
												   [this]()
												   {
													   if (InputManager::GetInstance().GetActionValue(ActionID::MoveV) < 0) // UP
													   {
														   _selectedIndex = (_selectedIndex - 1) < 0 ? _selectedIndex : _selectedIndex - 1;
													   }
													   else if (InputManager::GetInstance().GetActionValue(ActionID::MoveV) > 0) // DOWN
													   {
														   _selectedIndex = (_selectedIndex + 1) > 1 ? _selectedIndex : _selectedIndex + 1;
													   }
												   });


	sceneMng_.SetTransitionDuration(45.0f);
}

SceneTitle::~SceneTitle()
{

}

void SceneTitle::Update()
{
}

void SceneTitle::Draw()
{
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, _bgImg->GetHandle(), true);
	DrawRotaGraph(Application::SCREEN_WID / 2, LOGO_Y, 1.0f, 0.0f, _TitleLogoImg->GetHandle(), true);

	DrawRotaGraph(Application::SCREEN_WID / 2, START_Y, 1.0f, 0.0f, _TitleStartImg->GetHandle(), true);


	// ─── 選択肢の描画 (選択中なら黄色、それ以外は白) ───
	unsigned int color1P = (_selectedIndex == 0) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
	unsigned int color2P = (_selectedIndex == 1) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

	// 座標は画面レイアウトに合わせて微調整してください
	DrawString(Application::SCREEN_WID / 2 - 120, START_Y + 60, "1P START (HOST)", color1P);
	DrawString(Application::SCREEN_WID / 2 - 120, START_Y + 80, "2P START (CLIENT)", color2P);

	std::string TitleEndText = "Press Escape/Back to Exit";
	int textWidth = GetDrawStringWidth(TitleEndText.c_str(), TitleEndText.length());
	DrawString((Application::SCREEN_WID / 2) - (textWidth / 2), Application::SCREEN_HIG - 100, TitleEndText.c_str(), GetColor(255, 255, 255));

	DrawFormatString(700, 0, 0x00ff00, "selectedIndex:%d", _selectedIndex);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(_fadeAlpha));
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneTitle::TransitionIn(float t)
{
	float e = EaseInCubic(1 - t);
	_fadeAlpha = e * 255.0f;
}

void SceneTitle::TransitionOut(float t)
{
	_fadeAlpha = EaseOutCubic(t) * 255.0f;
}
