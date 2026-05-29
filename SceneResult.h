#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SceneManager.h"
enum class NextScene
{
	Next = 0,
	StageSelect,
	Title,
	Max
};

class SceneResult :
	public SceneSuper
{
private:
	static constexpr int LOGO_X = Application::SCREEN_WID / 2;
	static constexpr int LOGO_Y = 250;

	static constexpr int RESULT_TIME_X = 600;
	static constexpr int RESULT_TIME_Y = 500;

	static constexpr int NEXT_BUTTON_X = 310;
	static constexpr int NEXT_BUTTON_Y = 800;
	static constexpr int NEXT_BUTTON_WID = 400;
	static constexpr int NEXT_BUTTON_HIG = 150;
	static constexpr int NEXT_BUTTON_MARGIN = 50;
public:
	SceneResult(FileManager& fileMng, bool isClear, ClearResult& result);

	~SceneResult() override;

	void Update() override;
	void Draw() override;

	void MoveSelect(float moveValue);
private:
	bool _isClear;
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _resultClearLogoImg;
	std::shared_ptr<ImageFile> _resultFailedLogoImg;

	int resultFontHandle;
	int nextButtonFontHandle;

	ClearResult _clearResult;
	int _selectedNext;
};