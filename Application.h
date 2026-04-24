#pragma once
#include <memory>
class SceneManager;
class FileManager;

class Application
{
public:
	static constexpr int SCREEN_WID = 1920; // ゲーム画面の横サイズ
	static constexpr int SCREEN_HIG = 1080; // ゲーム画面の縦サイズ

	static constexpr const wchar_t* MOVE_UD_ACTION = L"MoveUD";
	static constexpr const wchar_t* MOVE_LR_ACTION = L"MoveLR";
	static constexpr const wchar_t* JUMP_ACTION = L"Jump";

	Application();
	~Application();

	void Run();
private:
	// Applicationのみ所有するように
	std::unique_ptr<SceneManager> sceneMng;
	std::unique_ptr<FileManager> fileMng;
	
	void Update();
	void Draw();
};
