#pragma once

class FileManager;

#include "SceneSuper.h"

class SceneManager
{
public:
	//enum class TransitionState
	//{
	//	None,
	//	FadeInNext,
	//	FadeOutCurrent,
	//	SwitchScene,
	//};

	//struct Transition
	//{
	//	TransitionState state = TransitionState::None;
	//	float timer = 0.0f;
	//	float duration = 45.0f; // フェードの総時間（フレーム数）
	//	SceneSuper::SceneID nextSceneID = SceneSuper::SceneID::NONE;
	//};
	SceneManager(FileManager& fileMng);
	~SceneManager();
	void Update();
	void Draw();

	bool GetExit() const;
	//void SetClearResult(const ClearResult& result);
private:
	void ChangeScene(SceneSuper::SceneID nextSceneID);
	//void UpdateTransition();
	std::unique_ptr<SceneSuper> currentScene; // 大元のSceneはかならず一つ
	FileManager& fileMng_;

	bool isExit;

	//Transition transition_;

	//ClearResult clearResult_;
};
