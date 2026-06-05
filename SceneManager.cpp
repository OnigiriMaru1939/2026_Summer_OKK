#include <DxLib.h>
#include <algorithm>

#include "Application.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "FileManager.h"

#include "SceneTitle.h"
#include "SceneStageSelect.h"
#include "SceneGame.h"
#include "ScenePause.h"
#include "SceneResult.h"

SceneManager::SceneManager(FileManager& fileMng) : fileMng_(fileMng)
{
	scenes.push_back(CreateScene(SceneSuper::SceneID::TITLE));
	isExit = false;
	_isClear = false;
}

SceneManager::~SceneManager()
{
	scenes.clear();
}

void SceneManager::Update()
{
	InputManager::GetInstance().Update();
	if (scenes.empty())
	{
		isExit = true;
		return;
	}

	auto& currentScene = scenes.back();
	currentScene->Update();

	if (currentScene->IsEnd())
	{
		ChangeScene(currentScene->GetNextScene());
	}
}

void SceneManager::Draw()
{
	if (scenes.empty()) return;

	for (const auto& scene : scenes)
	{
		scene->Draw();
	}
}

std::unique_ptr<SceneSuper> SceneManager::CreateScene(SceneSuper::SceneID sceneID)
{
	switch (sceneID)
	{
		case SceneSuper::SceneID::TITLE:
			return std::make_unique<SceneTitle>(fileMng_);
		case SceneSuper::SceneID::STAGE_SELECT:
			return std::make_unique<SceneStageSelect>(fileMng_);
		case SceneSuper::SceneID::GAME:
			return std::make_unique<SceneGame>(fileMng_, *this);
		case SceneSuper::SceneID::RESULT:
			return std::make_unique<SceneResult>(fileMng_, _isClear, _clearResult);
		case SceneSuper::SceneID::PAUSE:
			return std::make_unique<ScenePause>(fileMng_, *this);
		default:
			return nullptr;
	}
}

void SceneManager::ChangeScene(SceneSuper::SceneID nextSceneID)
{
	if (nextSceneID == SceneSuper::SceneID::EXIT)
	{
		isExit = true;
		return;
	}
	InputManager::GetInstance().ClearAxisCallbacks();
	InputManager::GetInstance().ClearPressCallbacks();
	InputManager::GetInstance().ClearTriggerCallbacks();
	InputManager::GetInstance().ClearReleaseCallbacks();

	auto newScene = CreateScene(nextSceneID);

	if (newScene != nullptr)
	{
		if (scenes.empty())
		{
			scenes.push_back(std::move(newScene));
		}
		else
		{
			scenes.back() = std::move(newScene);
		}
	}
	else
	{
#ifdef _DEBUG
		OutputDebugStringA("シーンの生成に失敗しました。\n");
#endif
	}
}

void SceneManager::PushScene(SceneSuper::SceneID sceneID)
{
	auto newScene = CreateScene(sceneID);
	if (newScene)
	{
		scenes.push_back(std::move(newScene));
	}
}

void SceneManager::PopScene()
{
	if (scenes.size() > 1)
	{
		scenes.pop_back();
	}
}

bool SceneManager::GetExit() const
{
	return isExit;
}

void SceneManager::SetClearResult(const ClearResult& result)
{
	_clearResult = result;
}
