#pragma once
#include "SceneSuper.h"
#include "SoundFile.h"
#include "ImageFile.h"
#include "FontFile.h"
#include "NetworkManager.h"
#include <memory>
#include <string>

class SceneManager;

enum class NextScenePause
{
	Back = 0,
	Retry = 1,
	StageSelect = 2,
	Title = 3,
	Exit = 4,
	Max
};

class ScenePause :
    public SceneSuper
{
private:
	static constexpr int BUTTON_X = 610;
	static constexpr int BUTTON_Y = 210;
	static constexpr int BUTTON_WID = 700;
	static constexpr int BUTTON_HIG = 100;
	static constexpr int BUTTON_MARGIN = 70;
public:
	ScenePause(FileManager& fileMng, SceneManager& sceneMng, const std::string& ip = "127.0.0.1");
	~ScenePause() override;
	void Update() override;
	void Draw() override;

	void SyncResume();
	void SyncChangeScene(int nextSceneIndex);
private:
	void DecidePauseScene(bool isFromNetwork = false);
	void MoveSelect(float moveValue);
	std::shared_ptr<ImageFile> _blockImg;
	std::shared_ptr<ImageFile> _blockSelectImg;
	std::shared_ptr<SoundFile> _decideSE;
	std::shared_ptr<SoundFile> _cursorSE;

	std::shared_ptr<FontFile> _textFont;
	std::shared_ptr<FontFile> _pauseFont;
	std::shared_ptr<FontFile> _explainFont;

	int _selectedIndex;

	NetworkManager& _networkMng;
};

