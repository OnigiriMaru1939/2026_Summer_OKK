#include "SceneTitle.h"
#include "InputManager.h"
#include "Application.h"

SceneTitle::SceneTitle(FileManager& fileMng) : SceneSuper(fileMng)
{
	_bgImg = fileMng_.LoadImageFM("Resource/Image/Title_bg_NoLogo.png");
	_TitleLogoImg = fileMng_.LoadImageFM("Resource/Image/Title_Logo.png");
	_TitleStartImg = fileMng_.LoadImageFM("Resource/Image/Title_Start.png");
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   SetNextScene(SceneID::STAGE_SELECT);
													   isEnd = true;
												   });
}

SceneTitle::~SceneTitle()
{

}

void SceneTitle::Update()
{
}

void SceneTitle::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	DrawString(0, 20, "TITLE SCENE", 0xffffff);
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, _bgImg->GetHandle(), true);
	DrawRotaGraph(Application::SCREEN_WID / 2, LOGO_Y, 1.0f, 0.0f, _TitleLogoImg->GetHandle(), true);
	DrawRotaGraph(Application::SCREEN_WID / 2, START_Y, 1.0f, 0.0f, _TitleStartImg->GetHandle(), true);
}