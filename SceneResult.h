#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SceneManager.h"
class SceneResult :
    public SceneSuper
{
private:
	static constexpr int LOGO_Y = 250;
	static constexpr int LOGO_X = Application::SCREEN_WID / 2;

	static constexpr int RESULT_TIME_X = 600;
	static constexpr int RESULT_TIME_Y = 500;
public:
	SceneResult(FileManager& fileMng, bool isClear, ClearResult& result);

	~SceneResult() override;

	void Update() override;
	void Draw() override;

private:
	bool _isClear;
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _resultClearLogoImg;
	std::shared_ptr<ImageFile> _resultFailedLogoImg;

	int resultFontHandle;

	ClearResult clearResult_;
};

