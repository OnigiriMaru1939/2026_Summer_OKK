#include "SceneGame.h"
#include "InputManager.h"

SceneGame::SceneGame(FileManager& fileMng) : SceneSuper(fileMng)
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::Update()
{
	if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
	{
		SetNextScene(SceneID::RESULT);
		isEnd = true;
	}
}

void SceneGame::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x00aa00, true);
	DrawString(0, 20, "GAME SCENE", 0xffffff);
}