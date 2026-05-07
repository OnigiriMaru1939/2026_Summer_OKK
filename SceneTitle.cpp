#include "SceneTitle.h"
#include "InputManager.h"
#include "Application.h"

SceneTitle::SceneTitle(FileManager& fileMng) : SceneSuper(fileMng)
{
	test = fileMng_.LoadImageFM("Resource/Image/Test_Miku.png");
}

SceneTitle::~SceneTitle()
{

}

void SceneTitle::Update()
{
	if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
	{
		SetNextScene(SceneID::STAGE_SELECT);
		isEnd = true;
	}
}

void SceneTitle::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x000000, true);
	DrawString(0, 20, "TITLE SCENE", 0xffffff);
	DrawRotaGraph(Application::SCREEN_WID / 2, Application::SCREEN_HIG / 2, 1.0f, 0.0f, test->GetHandle(), true);
}