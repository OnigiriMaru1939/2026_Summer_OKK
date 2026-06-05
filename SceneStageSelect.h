#pragma once
#include "SceneSuper.h"
#include "Application.h"
#include "ImageFile.h"
#include "SoundFile.h"

#include <memory>
class SceneStageSelect :
    public SceneSuper
{
private:
	static constexpr int H1_X = 860;
	static constexpr int H1_Y = 130;

	static constexpr int BLOCK_NUM_ROW = 2; // ブロックの横列数
	static constexpr int BLOCK_NUM_COL = 2; // ブロックの縦列数
	static constexpr int BLOCK_NUM = BLOCK_NUM_ROW * BLOCK_NUM_COL; // ブロックの総数

	static constexpr int BLOCK_X = 1020;
	static constexpr int BLOCK_Y = 300;
	static constexpr int BLOCK_SPACING_X = 20; // ブロック間の横スペース
	static constexpr int BLOCK_SPACING_Y = 20; // ブロック間の縦スペース

	static constexpr int TITLE_X = 100;
	static constexpr int TITLE_Y = 830;
	static constexpr int TITLE_W = 400;	// 仮タイトルボタン幅
	static constexpr int TITLE_H = 150;	// 仮タイトルボタン高さ
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

	std::shared_ptr<SoundFile> _decideSE;
	std::shared_ptr<SoundFile> _cursorSE;

	int _selectedButton; // 0がタイトル、1～4がステージ選択

	int stageFontHandle;
	int titleFontHandle;
	int stageNumFontHandle;
};

