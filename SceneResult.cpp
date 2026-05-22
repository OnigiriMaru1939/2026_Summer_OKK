#include "SceneResult.h"
#include "InputManager.h"

SceneResult::SceneResult(FileManager& fileMng) : SceneSuper(fileMng)
{
	_bgImg = fileMng.LoadImageFM("Resource/Image/Title_bg_NoLogo.png");
	_resultLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Clear_Logo.png");
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   SetNextScene(SceneID::TITLE);
													   isEnd = true;
												   });
}

SceneResult::~SceneResult()
{

}

void SceneResult::Update()
{

}

void SceneResult::Draw()
{
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, _bgImg->GetHandle(), true);
	DrawRotaGraph(LOGO_X, LOGO_Y, 1.0f, -5.0f * (DX_PI_F / 180.0f), _resultLogoImg->GetHandle(), true);
}