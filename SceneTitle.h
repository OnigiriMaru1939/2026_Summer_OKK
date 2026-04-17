#pragma once
#include "SceneSuper.h"
#include "Application.h"

class SceneTitle :
    public SceneSuper
{
public:
	SceneTitle(FileManager& fileMng);

	~SceneTitle() override;

	void Update() override;
	void Draw() override;
};

