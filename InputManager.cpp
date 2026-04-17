#include <DxLib.h>

#include "InputManager.h"

InputManager::InputManager()
{
	// 配列の初期化を明示
	memset(key, 0, sizeof(key));
	memset(prevKey, 0, sizeof(prevKey));
	memset(mouseButton, 0, sizeof(mouseButton));
	memset(prevMouseButton, 0, sizeof(prevMouseButton));
	mouseX = mouseY = prevMouseX = prevMouseY = 0;
}

void InputManager::Update()
{
	// キー入力
	for (int i = 0; i < 256; i++)
	{
		prevKey[i] = key[i];
	}
	char tmpKey[256];
	GetHitKeyStateAll(tmpKey);
	for (int i = 0; i < 256; i++)
	{
		key[i] = (tmpKey[i] != 0) ? key[i] + 1 : 0;
	}

	// マウス座標
	prevMouseX = mouseX;
	prevMouseY = mouseY;
	GetMousePoint(&mouseX, &mouseY);

	// マウスボタン
	for (int i = 0; i < 3; i++)
	{
		prevMouseButton[i] = mouseButton[i];
	}

	int mouseInput = GetMouseInput();

	auto UpdateMouse = [&](int index, int mask)
		{
			if (mouseInput & mask)
			{
				mouseButton[index]++;
			}
			else
			{
				mouseButton[index] = 0;
			}
		};

	UpdateMouse(0, MOUSE_INPUT_LEFT);
	UpdateMouse(1, MOUSE_INPUT_RIGHT);
	UpdateMouse(2, MOUSE_INPUT_MIDDLE);
}

bool InputManager::IsKeyPressed(int keyCode) const
{
	return key[keyCode] > 0;
}

bool InputManager::IsKeyTriggered(int keyCode) const
{
	return key[keyCode] == 1;
}

bool InputManager::IsKeyReleased(int keyCode) const
{
	return prevKey[keyCode] > 0 && key[keyCode] == 0;
}

bool InputManager::IsMousePressed(MouseButton button) const
{
	return mouseButton[(int)button] > 0;
}

bool InputManager::IsMouseTriggered(MouseButton button) const
{
	return mouseButton[(int)button] == 1;
}

bool InputManager::IsMouseReleased(MouseButton button) const
{
	return mouseButton[(int)button] == 0 && prevMouseButton[(int)button] > 0;
}
