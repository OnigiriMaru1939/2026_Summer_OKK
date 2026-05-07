#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include <memory>

class SceneTitle :
    public SceneSuper
{
public:
	SceneTitle(FileManager& fileMng);

	~SceneTitle() override;

	void Update() override;
	void Draw() override;
private:
	std::shared_ptr<ImageFile> test;
};

