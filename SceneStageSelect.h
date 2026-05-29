#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"

#include <memory>
class SceneStageSelect :
    public SceneSuper
{
private:
	static constexpr int H1_X = 860;
	static constexpr int H1_Y = 130;

	static constexpr int BLOCK_X = 1020;
	static constexpr int BLOCK_Y = 300;
public:
	SceneStageSelect(FileManager& fileMng);

	~SceneStageSelect() override;

	void Update() override;
	void Draw() override;

	void MoveSelect(float moveValue, bool isHorizontal);

private:
	std::shared_ptr<ImageFile> _bgImg;
	std::shared_ptr<ImageFile> _stageSelectH1Img;
	std::shared_ptr<ImageFile> _stageSelectBlockImg;

	int _selectedStage;

	int stageFontHandle;
	int stageNumFontHandle;
};

