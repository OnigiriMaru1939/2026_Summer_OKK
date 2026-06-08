#pragma once
#include "SceneSuper.h"
#include "SoundFile.h"
#include <memory>
class SceneManager;

enum class NextScenePause
{
	Back = 0,
	Retry,
	StageSelect,
	Title,
	Exit,
	Max
};

class ScenePause :
    public SceneSuper
{
private:
	static constexpr int BUTTON_X = 610;
	static constexpr int BUTTON_Y = 150;
	static constexpr int BUTTON_WID = 700;
	static constexpr int BUTTON_HIG = 100;
	static constexpr int BUTTON_MARGIN = 70;
public:
	ScenePause(FileManager& fileMng, SceneManager& sceneMng);
	~ScenePause() override;
	void Update() override;
	void Draw() override;
private:
	void MoveSelect(float moveValue);
	void DecidePauseScene();
	std::shared_ptr<SoundFile> _decideSE;
	std::shared_ptr<SoundFile> _cursorSE;

	int _selectedIndex;

	SceneManager& sceneMng_; // シーンマネージャーへの弱い参照
};

