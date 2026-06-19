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
														   SetNextScene(SceneID::STAGE_SELECT);
														   isEnd = true;
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
	std::string TitleEndText = "Press Escape/Back to Exit";
	int textWidth = GetDrawStringWidth(TitleEndText.c_str(), TitleEndText.length());
	DrawString((Application::SCREEN_WID / 2) - (textWidth / 2), Application::SCREEN_HIG - 100, TitleEndText.c_str(), GetColor(255, 255, 255));

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
