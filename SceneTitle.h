#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include "SceneManager.h"
#include <memory>
#include <string>

class SceneTitle :
    public SceneSuper
{
private:
	static constexpr int LOGO_Y = 360;
	static constexpr int START_Y = 960;

	static constexpr int MAX_INDEX = 2; // 0:1P, 1:2P, 2:IP入力
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
	bool _isInputActive = false;

	char myIPBuffer[10][32] = { "" };
	int ipCount = 0;

	std::string _inputIp = "127.0.0.1";
};

