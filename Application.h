#pragma once
#include <memory>
#include "ActionID.h"
class SceneManager;
class FileManager;

class Application
{
public:
	static constexpr int SCREEN_WID = 1920; // ゲーム画面の横サイズ
	static constexpr int SCREEN_HIG = 1080; // ゲーム画面の縦サイズ

	Application();
	~Application();

	void Run();
private:
	// デバッグ用の入力
	float moveSpeed = 5.0f;
	float x = 0;
	float y = 0;
	int color = 0xffffff;
	void DebugMoveX();
	void DebugMoveY();
	void DeBugJump();
	// Applicationのみ所有するように
	std::unique_ptr<SceneManager> sceneMng;
	std::unique_ptr<FileManager> fileMng;
	
	void Update();
	void Draw();
};
