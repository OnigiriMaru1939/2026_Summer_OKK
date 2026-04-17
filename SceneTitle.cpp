#include "SceneTitle.h"
#include "InputManager.h"

SceneTitle::SceneTitle(FileManager& fileMng) : SceneSuper(fileMng)
{

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
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x0000aa, true);
	DrawString(0, 20, "TITLE SCENE", 0xffffff);
}