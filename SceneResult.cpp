#include "SceneResult.h"
#include "InputManager.h"

SceneResult::SceneResult(FileManager& fileMng) : SceneSuper(fileMng)
{

}

SceneResult::~SceneResult()
{

}

void SceneResult::Update()
{
	if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
	{
		SetNextScene(SceneID::TITLE);
		isEnd = true;
	}
}

void SceneResult::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0xaa0000, true);
	DrawString(0, 20, "RESULT SCENE", 0xffffff);
}