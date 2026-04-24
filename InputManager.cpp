#include <DxLib.h>

#include "InputManager.h"

void InputManager::DrawDebug(int x, int y) const
{
	int curY = y;
	int color = GetColor(255, 255, 255);
	int activeColor = GetColor(255, 255, 0); // 押されている時は黄色

	// 1. キーボード情報 (押されているキーのみ表示)
	DrawString(x, curY, "--- Keyboard (Active Keys) ---", color);
	curY += 20;
	for (int i = 0; i < 256; i++)
	{
		if (key[i] > 0)
		{
			DrawFormatString(x, curY, activeColor, "Key[%d]: %d frames", i, key[i]);
			curY += 20;
		}
	}

	// 2. マウス情報
	DrawFormatString(x, curY, color, "--- Mouse: (%d, %d) ---", mouseX, mouseY);
	curY += 20;
	for (int i = 0; i < 3; i++)
	{
		if (mouseButton[i] > 0)
		{
			DrawFormatString(x, curY, activeColor, "MB[%d]: %d frames", i, mouseButton[i]);
			curY += 20;
		}
	}

	// 3. ジョイパッド情報
	for (int no = 0; no < MAX_JOYPADS; no++)
	{
		int dxNo = no + 1;
		// パッドが接続されているか確認
		char deviceName[260];
		char productName[260];
		if (GetJoypadName(dxNo, &deviceName[0], &productName[0]) == -1) continue;

		int type = GetJoypadType(dxNo);
		DrawFormatString(x, curY, color, "--- Pad %d [%s] ---", no, GetPadName(type));
		curY += 20;

		// スティック値
		DrawFormatString(x, curY, color, " L-Stick: %d, %d", padLX[no], padLY[no]);
		curY += 18;
		DrawFormatString(x, curY, color, " R-Stick: %d, %d", padRX[no], padRY[no]);
		curY += 20;

		// ボタン状態 (押されているボタンのみ表示)
		const char* btnNames[] = { "F_Left", "F_Right", "F_Top", "F_Down", "Up", "Down", "Left", "Right", "L_S", "R_S", "L_T", "R_T" };
		for (int b = 0; b < (int)PadButton::Max; b++)
		{
			if (padButton[no][b] > 0)
			{
				DrawFormatString(x + 10, curY, activeColor, "%s: %d frames", btnNames[b], padButton[no][b]);
				curY += 18;
			}
		}
		curY += 10;
	}
}

const char* InputManager::GetPadName(int type) const
{
	switch (type)
	{
		case (int)JOYPAD_TYPE::OTHER: return "Other";
		case (int)JOYPAD_TYPE::XBOX_360: return "Xbox 360";
		case (int)JOYPAD_TYPE::XBOX_ONE: return "Xbox One";
		case (int)JOYPAD_TYPE::DUAL_SHOCK_4: return "DualShock 4";
		case (int)JOYPAD_TYPE::DUAL_SENSE: return "DualSense";
		case (int)JOYPAD_TYPE::SWITCH_JOY_CON_L: return "Switch Joy-Con L";
		case (int)JOYPAD_TYPE::SWITCH_JOY_CON_R: return "Switch Joy-Con R";
		case (int)JOYPAD_TYPE::SWITCH_PRO_CTRL: return "Switch Pro Controller";
		default: return "Unknown";
	}
}

InputManager::InputManager()
{
	// 配列の初期化を明示
	memset(key, 0, sizeof(key));
	memset(prevKey, 0, sizeof(prevKey));
	memset(padButton, 0, sizeof(padButton));
	memset(prevPadButton, 0, sizeof(prevPadButton));
	memset(padLX, 0, sizeof(padLX));
	memset(padLY, 0, sizeof(padLY));
	memset(padRX, 0, sizeof(padRX));
	memset(padRY, 0, sizeof(padRY));
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
	int mouseInput = GetMouseInput();
	for (int i = 0; i < 3; i++)
	{
		prevMouseButton[i] = mouseButton[i];
	}

	mouseButton[0] = (mouseInput & MOUSE_INPUT_LEFT) ? mouseButton[0] + 1 : 0;
	mouseButton[1] = (mouseInput & MOUSE_INPUT_RIGHT) ? mouseButton[1] + 1 : 0;
	mouseButton[2] = (mouseInput & MOUSE_INPUT_MIDDLE) ? mouseButton[2] + 1 : 0;

	// コントローラー
	for (int no = 0; no < MAX_JOYPADS; no++)
	{
		int dxNo = no + 1; // DxLibのジョイパッド番号は1から始まる
		int type = GetJoypadType(dxNo);
		DINPUT_JOYSTATE dState;
		XINPUT_STATE xState;
		GetJoypadDirectInputState(dxNo, &dState);
		GetJoypadXInputState(dxNo, &xState);

		padLX[no] = dState.X;
		padLY[no] = dState.Y;
		if (type == DX_PADTYPE_XBOX_360 || type == DX_PADTYPE_XBOX_ONE)
		{
			padRX[no] = dState.Rx;
			padRY[no] = dState.Ry;
		}
		else
		{
			padRX[no] = dState.Z;
			padRY[no] = dState.Rz;
		}

		for (int btn = 0; btn < (int)PadButton::Max; btn++)
		{
			prevPadButton[no][btn] = padButton[no][btn];
			if (GetRawState(no, (PadButton)btn))
			{
				padButton[no][btn]++;
			}
			else
			{
				padButton[no][btn] = 0;
			}
		}
	}
}

bool InputManager::GetRawState(int no, PadButton btn)
{
	int dxNo = no + 1;
	int type = GetJoypadType(dxNo);
	DINPUT_JOYSTATE dState;
	XINPUT_STATE xState;
	GetJoypadDirectInputState(dxNo, &dState);
	GetJoypadXInputState(dxNo, &xState);

	switch (btn)
	{
		case PadButton::Face_Left: // Xbox:X, PS:□
			if (type == DX_PADTYPE_DUAL_SENSE) return dState.Buttons[0] != 0;
			return (dState.Buttons[2] != 0) || (xState.Buttons[XINPUT_BUTTON_X] != 0);

		case PadButton::Face_Right: // Xbox:B, PS:〇
			if (type == DX_PADTYPE_DUAL_SENSE) return dState.Buttons[2] != 0;
			return (dState.Buttons[1] != 0) || (xState.Buttons[XINPUT_BUTTON_B] != 0);

		case PadButton::Face_Top: // Xbox:Y, PS:△
			return (dState.Buttons[3] != 0) || (xState.Buttons[XINPUT_BUTTON_Y] != 0);

		case PadButton::Face_Down: // Xbox:A, PS:×
			if (type == DX_PADTYPE_DUAL_SENSE) return dState.Buttons[1] != 0;
			return (dState.Buttons[0] != 0) || (xState.Buttons[XINPUT_BUTTON_A] != 0);

		case PadButton::Left:
			return (dState.POV[0] == 27000) || (xState.Buttons[XINPUT_BUTTON_DPAD_LEFT] != 0);
		case PadButton::Right:
			return (dState.POV[0] == 9000) || (xState.Buttons[XINPUT_BUTTON_DPAD_RIGHT] != 0);
		case PadButton::Top:
			return (dState.POV[0] == 0) || (xState.Buttons[XINPUT_BUTTON_DPAD_UP] != 0);
		case PadButton::Down:
			return (dState.POV[0] == 18000) || (xState.Buttons[XINPUT_BUTTON_DPAD_DOWN] != 0);

		case PadButton::L_Shoulder: return (dState.Buttons[4] != 0) || (xState.Buttons[XINPUT_BUTTON_LEFT_SHOULDER] != 0);
		case PadButton::R_Shoulder: return (dState.Buttons[5] != 0) || (xState.Buttons[XINPUT_BUTTON_RIGHT_SHOULDER] != 0);

		case PadButton::L_Trigger:
			// XInputは0-255の値が来るので、30以上を「押し込み」と判定
			return (xState.LeftTrigger > 30) || (dState.Buttons[6] != 0);
		case PadButton::R_Trigger:
			return (xState.RightTrigger > 30) || (dState.Buttons[7] != 0);

		default: return false;
	}
}