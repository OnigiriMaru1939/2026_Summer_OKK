#pragma once
#include <DxLib.h>
#include "FileManager.h"

class SceneSuper
{
public:
	enum class SceneID
	{
		NONE = -1,
		TITLE,
		STAGE_SELECT,
		GAME,
		RESULT,
		EXIT,
	};

	// SceneからFileManagerを消さないようにポインタ参照
	SceneSuper(FileManager& fileMng) : fileMng_(fileMng) {};
	virtual ~SceneSuper() = default;
	// 戻り値: true = シーン継続, false = シーン終了
	virtual void Update() = 0;
	virtual void Draw() = 0;

	bool IsEnd() const { return isEnd; }
	SceneID GetNextScene() const { return nextSceneID; }
	void SetNextScene(SceneID next) { nextSceneID = next; }
	float EaseInOut(float t) { return t * t * (3.0f - 2.0f * t); }

	virtual void SetTransitionIn(float t) {}
	virtual void SetTransitionOut(float t) {}
	void SetIsTransition(bool b) { isTransition = b; }
	bool GetIsTransition() { return isTransition; }
protected:
	FileManager& fileMng_;
	bool isEnd = false;
	bool isTransition = false;
	SceneID nextSceneID = SceneID::NONE;
};
