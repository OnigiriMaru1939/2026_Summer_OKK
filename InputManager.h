#pragma once

enum class MouseButton
{
	Left = 0,
	Right,
	Middle
};

class InputManager
{
private:
	InputManager();
	~InputManager() {};

public:
	// シングルトン
	static InputManager& GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	void Update();

	bool IsKeyPressed(int keyCode) const;
	bool IsKeyTriggered(int keyCode) const;
	bool IsKeyReleased(int keyCode) const;

	// マウス
	bool IsMousePressed(MouseButton button) const;
	bool IsMouseTriggered(MouseButton button) const;
	bool IsMouseReleased(MouseButton button) const;
	int GetMouseX() const { return mouseX; }
	int GetMouseY() const { return mouseY; }
	int GetMouseDX() const { return mouseX - prevMouseX; }
	int GetMouseDY() const { return mouseY - prevMouseY; }

private:
	int key[256] = {};
	int prevKey[256] = {};

	int mouseButton[3] = {}; // 0:左 1:右 2:中 
	int prevMouseButton[3] = {};

	int mouseX;
	int mouseY;
	int prevMouseX;
	int prevMouseY;
};
