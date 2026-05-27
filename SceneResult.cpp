#include "SceneResult.h"
#include "InputManager.h"

SceneResult::SceneResult(FileManager& fileMng, bool isClear, ClearResult& result) : SceneSuper(fileMng), _isClear(isClear), clearResult_(result)
{
	AddFontResourceExA("Resource/fonts/DotGothic16-Regular.ttf", FR_PRIVATE, NULL);
	resultFontHandle = CreateFontToHandle("DotGothic16", 80, -1, DX_FONTTYPE_NORMAL);

	_bgImg = fileMng.LoadImageFM("Resource/Image/Title_bg_NoLogo.png");
	_resultClearLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Clear_Logo.png");
	_resultFailedLogoImg = fileMng.LoadImageFM("Resource/Image/Result/Result_Failed_Logo.png");
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   SetNextScene(SceneID::TITLE);
													   isEnd = true;
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
		DrawFormatStringToHandle(RESULT_TIME_X, RESULT_TIME_Y, 0xffffff, resultFontHandle, "CLEAR TIME: %.2f", clearResult_.time);
	}
	else
	{
		DrawRotaGraph(LOGO_X, LOGO_Y, 1.0f, -5.0f * (DX_PI_F / 180.0f), _resultFailedLogoImg->GetHandle(), true);
	}
}