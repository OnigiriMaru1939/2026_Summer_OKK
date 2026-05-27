#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
class SceneResult :
    public SceneSuper
{
private:
	static constexpr int LOGO_Y = 250;
	static constexpr int LOGO_X = Application::SCREEN_WID / 2;
public:
	SceneResult(FileManager& fileMng, bool isClear);

	~SceneResult() override;

	void Update() override;
	void Draw() override;

private:
	bool _isClear;
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _resultClearLogoImg;
	std::shared_ptr<ImageFile> _resultFailedLogoImg;
};

