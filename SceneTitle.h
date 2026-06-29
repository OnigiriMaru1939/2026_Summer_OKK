#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include "SceneManager.h"
#include <memory>

class SceneTitle :
    public SceneSuper
{
private:
	static constexpr int LOGO_Y = 400;
	static constexpr int START_Y = 840;
public:
	SceneTitle(FileManager& fileMng, SceneManager& sceneMng);

	~SceneTitle() override;

	void Update() override;
	void Draw() override;

	void TransitionIn(float t) override;
	void TransitionOut(float t) override;
private:
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _TitleLogoImg;
	std::shared_ptr<ImageFile> _TitleStartImg;

	std::shared_ptr<SoundFile> _mainBgm;
	std::shared_ptr<SoundFile> _decideSE;

	float _fadeAlpha;

	int _selectedIndex = 0;
};

