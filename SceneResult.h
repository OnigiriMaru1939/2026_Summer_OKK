#pragma once
#include "SceneSuper.h"
#include "Application.h"
class SceneResult :
    public SceneSuper
{
public:
	SceneResult(FileManager& fileMng);

	~SceneResult() override;

	void Update() override;
	void Draw() override;
};

