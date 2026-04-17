#include <DxLib.h>
#include <algorithm>

#include "Application.h"

#include "SceneManager.h"
#include "InputManager.h"
#include "FileManager.h"

#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneGame.h"
#include "SceneResult.h"

SceneManager::SceneManager(FileManager& fileMng) : fileMng_(fileMng)
{
	currentScene = std::make_unique<SceneTitle>(fileMng_);
	isExit = false;
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	currentScene->Update();
	if (currentScene->IsEnd())
	{
		ChangeScene(currentScene->GetNextScene());
	}
}

void SceneManager::Draw()
{
	currentScene->Draw();
}

void SceneManager::ChangeScene(SceneSuper::SceneID nextSceneID)
{
	switch (nextSceneID)
	{
		case SceneSuper::SceneID::TITLE:
			currentScene = std::make_unique<SceneTitle>(fileMng_);
			break;
		case SceneSuper::SceneID::STAGE_SELECT:
			currentScene = std::make_unique<SceneStageSelect>(fileMng_);
			break;
		case SceneSuper::SceneID::GAME:
			currentScene = std::make_unique<SceneGame>(fileMng_);
			break;
		case SceneSuper::SceneID::RESULT:
			currentScene = std::make_unique<SceneResult>(fileMng_);
			break;
		case SceneSuper::SceneID::EXIT:
			isExit = true;
			break;
		default:
#ifdef _DEBUG
			OutputDebugStringA("Unknown SceneID\n");
#endif
			break;
	}
}

bool SceneManager::GetExit() const
{
	return isExit;
}