#pragma once
#include "SceneSuper.h"
class SceneManager;
class ScenePause :
    public SceneSuper
{
	public:
	ScenePause(FileManager& fileMng, SceneManager& sceneMng);
	~ScenePause() override;
	void Update() override;
	void Draw() override;
private:

	SceneManager& sceneMng_; // シーンマネージャーへの弱い参照
};

