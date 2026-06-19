#pragma once
#include "SceneSuper.h"
#include "SoundFile.h"
#include "ImageFile.h"
#include "FontFile.h"
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
	static constexpr int BUTTON_Y = 210;
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
	std::shared_ptr<ImageFile> _blockImg;
	std::shared_ptr<ImageFile> _blockSelectImg;
	std::shared_ptr<SoundFile> _decideSE;
	std::shared_ptr<SoundFile> _cursorSE;

	std::shared_ptr<FontFile> _textFont;
	std::shared_ptr<FontFile> _pauseFont;
	std::shared_ptr<FontFile> _explainFont;

	int _selectedIndex;
};

