#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include <memory>
#include <memory>

class SceneTitle :
    public SceneSuper
{
private:
	static constexpr int LOGO_Y = 400;
	static constexpr int START_Y = 880;
public:
	SceneTitle(FileManager& fileMng);

	~SceneTitle() override;

	void Update() override;
	void Draw() override;
private:
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _TitleLogoImg;
	std::shared_ptr<ImageFile> _TitleStartImg;

	std::shared_ptr<SoundFile> _mainBgm;
};

