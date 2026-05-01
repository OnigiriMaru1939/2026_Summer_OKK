#include "SceneStageSelect.h"
#include "InputManager.h"

SceneStageSelect::SceneStageSelect(FileManager& fileMng) : SceneSuper(fileMng)
{
}

SceneStageSelect::~SceneStageSelect()
{

}

void SceneStageSelect::Update()
{
	if (InputManager::GetInstance().IsKeyTriggered(KEY_INPUT_RETURN))
	{
		SetNextScene(SceneID::GAME);
		isEnd = true;
	}
}

void SceneStageSelect::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x006666, true);
	DrawString(0, 20, "STAGESELECT SCENE", 0xffffff);
}