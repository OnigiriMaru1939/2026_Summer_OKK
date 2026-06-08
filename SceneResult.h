#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include "SceneManager.h"
enum class NextScene
{
	Next = 0,
	Retry,
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

	static constexpr int NEXT_BUTTON_X = 95;
	static constexpr int NEXT_BUTTON_Y = 800;
	static constexpr int NEXT_BUTTON_WID = 400;
	static constexpr int NEXT_BUTTON_HIG = 150;
	static constexpr int NEXT_BUTTON_MARGIN = 50;
public:
	SceneResult(FileManager& fileMng, bool isClear, ClearResult& result);

	~SceneResult() override;

	void Update() override;
	void Draw() override;

	void TransitionIn(float t) override;
private:
	bool _isClear;
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _resultClearLogoImg;
	std::shared_ptr<ImageFile> _resultFailedLogoImg;

	std::shared_ptr<SoundFile> _decideSE;
	std::shared_ptr<SoundFile> _cursorSE;

	int resultFontHandle;
	int nextButtonFontHandle;

	ClearResult _clearResult;
	int _selectedNext;

	float _fadeAlpha;

	void MoveSelect(float moveValue);
};