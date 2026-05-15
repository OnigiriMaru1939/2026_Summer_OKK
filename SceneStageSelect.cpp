#include "SceneStageSelect.h"
#include "InputManager.h"

SceneStageSelect::SceneStageSelect(FileManager& fileMng) : SceneSuper(fileMng)
{
	InputManager::GetInstance().SetTriggerCallback(ActionID::Decide,
												   [this]()
												   {
													   SetNextScene(SceneID::GAME);
													   isEnd = true;
												   });
}

SceneStageSelect::~SceneStageSelect()
{

}

void SceneStageSelect::Update()
{

}

void SceneStageSelect::Draw()
{
	DrawBox(0, 0, Application::SCREEN_WID, Application::SCREEN_HIG, 0x006666, true);
	DrawString(0, 20, "STAGESELECT SCENE", 0xffffff);
}