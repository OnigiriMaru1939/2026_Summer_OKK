#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include "FontFile.h"
#include "SceneManager.h"
#include "NetworkManager.h"
#include <string>

enum class NextScene
{
	Next = 0,
	Retry = 1,
	StageSelect = 2,
	Title = 3,
	Result = 4,
	Exit = 5,
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
	SceneResult(FileManager& fileMng, bool isClear, ClearResult& result, SceneManager& sceneMng, const std::string& ip = "127.0.0.1");

	~SceneResult() override;

	void Update() override;
	void Draw() override;

	void TransitionIn(float t) override;
	void TransitionOut(float t) override;
private:
	bool _isClear;
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _resultClearLogoImg;
	std::shared_ptr<ImageFile> _resultFailedLogoImg;

	std::shared_ptr<SoundFile> _decideSE;
	std::shared_ptr<SoundFile> _cursorSE;

	std::shared_ptr<FontFile> _resultFont;
	std::shared_ptr<FontFile> _nextButtonFont;

	ClearResult _clearResult;
	int _selectedNext;

	float _fadeInAlpha;
	float _fadeOutAlpha;

	NetworkManager& _networkMng;

	void MoveSelect(float moveValue);
};