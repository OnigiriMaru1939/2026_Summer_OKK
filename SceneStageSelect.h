#pragma once
#include "SceneSuper.h"
#include "Application.h"
class SceneStageSelect :
    public SceneSuper
{
public:
	SceneStageSelect(FileManager& fileMng);

	~SceneStageSelect() override;

	void Update() override;
	void Draw() override;
};

