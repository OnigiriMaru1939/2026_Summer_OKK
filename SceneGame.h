#pragma once
#include "SceneSuper.h"
#include "Application.h"
class SceneGame :
    public SceneSuper
{
public:
	SceneGame(FileManager& fileMng);

	~SceneGame() override;

	void Update() override;
	void Draw() override;
};

