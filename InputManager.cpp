#include <DxLib.h>

#include "InputManager.h"
#include "KeyConfig.h"

// デバッグ表示用の関数
void InputManager::DrawDebug(int x, int y) const
{
	int curY = y;
	int color = GetColor(255, 255, 255);
	int activeColor = GetColor(255, 255, 0); // 押されている時は黄色

	// 1. キーボード情報 (押されているキーのみ表示)
	DrawString(x, curY, "--- Keyboard (Active Keys) ---", color);
	curY += 20;
	for (int i = 0; i < KEY_COUNT; i++)
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
	for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
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
		const char* btnNames[] = { "F_Left", "F_Right", "F_Top", "F_Down", "Up", "Down", "Left", "Right", "L_S", "R_S", "L_T", "R_T", "L_StickClick", "R_StickClick", "Start", "Back"};
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

// ジョイパッドのタイプに応じた名前を返す関数
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

// コントローラーのボタンマッピングを解決する関数
InputManager::InputManager()
{
	// 配列の初期化を明示 (必須ではないが、可読性向上のため)
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

InputManager::~InputManager()
{
	ClearTriggerCallbacks();
	ClearPressCallbacks();
	ClearReleaseCallbacks();
}

void InputManager::Update()
{
	// キー入力
	for (int i = 0; i < KEY_COUNT; i++)
	{
		prevKey[i] = key[i];
	}
	char tmpKey[KEY_COUNT];
	GetHitKeyStateAll(tmpKey);
	for (int i = 0; i < KEY_COUNT; i++)
	{
		key[i] = (tmpKey[i] != 0) ? key[i] + 1 : 0;
	}

	// マウス座標
	prevMouseX = mouseX;
	prevMouseY = mouseY;
	GetMousePoint(&mouseX, &mouseY);

	// マウスボタン
	int mouseInput = GetMouseInput();
	for (int i = 0; i < MOUSE_BUTTON_COUNT; i++)
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
		// XboxコントローラーはRx/Ryが右スティック
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
	DispatchCallbacks();
}

// キーボード入力
bool InputManager::IsKeyPressed(int keyCode) const
{
	return IsInputPressed(key[keyCode]);
}
bool InputManager::IsKeyTriggered(int keyCode) const
{
	return IsInputTriggered(key[keyCode], prevKey[keyCode]);
}
bool InputManager::IsKeyReleased(int keyCode) const
{
	return IsInputReleased(key[keyCode], prevKey[keyCode]);
}

// マウス入力
bool InputManager::IsMousePressed(MouseButton button) const
{
	return IsInputPressed(mouseButton[(int)button]);
}
bool InputManager::IsMouseTriggered(MouseButton button) const
{
	return IsInputTriggered(mouseButton[(int)button], prevMouseButton[(int)button]);
}
bool InputManager::IsMouseReleased(MouseButton button) const
{
	return IsInputReleased(mouseButton[(int)button], prevMouseButton[(int)button]);
}

// コントローラー入力
bool InputManager::IsPadPressed(int no, PadButton btn) const
{
	return IsInputPressed(padButton[no][(int)btn]);
}
bool InputManager::IsPadTriggered(int no, PadButton btn) const
{
	return IsInputTriggered(padButton[no][(int)btn], prevPadButton[no][(int)btn]);
}
bool InputManager::IsPadReleased(int no, PadButton btn) const
{
	return IsInputReleased(padButton[no][(int)btn], prevPadButton[no][(int)btn]);
}

// スティックの値を-1.0f〜1.0fの範囲で取得する関数
float InputManager::GetAxisValue(int padNo, PadAxis axis) const
{
	int rawValue = 0;
	switch (axis)
	{
		case PadAxis::Pad_L_X: rawValue = padLX[padNo]; break;
		case PadAxis::Pad_L_Y: rawValue = padLY[padNo]; break;
		case PadAxis::Pad_R_X: rawValue = padRX[padNo]; break;
		case PadAxis::Pad_R_Y: rawValue = padRY[padNo]; break;
	}

	if (abs(rawValue) < DEADZONE) return 0.0f; // デッドゾーン内は0とみなす

	float normalized = (float)(abs(rawValue) - DEADZONE) / (STICK_MAX - DEADZONE);
	return (rawValue > 0) ? normalized : -normalized;
}

// アクションに対する入力値を取得する関数
float InputManager::GetActionValue(ActionID action, int padNo) const
{
	const auto& mappings = KeyConfig::GetInstance().GetMappings(action);
	if (mappings.empty()) return 0.0f; // アクションにマッピングがない場合は0を返す

	float total = 0.0f;

	for (const auto& m : mappings)
	{
		float val = 0.0f;
		switch (m.inputType)
		{
			case InputType::Keyboard:
				val = IsKeyPressed(m.code) ? 1.0f : 0.0f;
				break;
			case InputType::GamepadButton:
				val = IsPadPressed(padNo, (PadButton)m.code) ? 1.0f : 0.0f;
				break;
			case InputType::GamepadAxis:
				val = GetAxisValue(padNo, (PadAxis)m.code);
				break;
			case InputType::Mouse:
				val = IsMousePressed((MouseButton)m.code) ? 1.0f : 0.0f;
				break;
		}
		total += val * m.scale; // スケールをかけて加算
	}

	// 複数の入力がある場合は合算するが、値が-1.0f〜1.0fの範囲を超えないようにクランプする
	return (total > 1.0f) ? 1.0f : (total < -1.0f) ? -1.0f : total;
}

// アクションに対する入力状態を取得する関数
bool InputManager::IsActionPressed(ActionID action, int padNo) const
{
	const auto& mappings = KeyConfig::GetInstance().GetMappings(action);
	for (const auto& m : mappings)
	{
		switch (m.inputType)
		{
			case InputType::Keyboard:
				if (IsKeyPressed(m.code)) return true;
				break;
			case InputType::GamepadButton:
				if (IsPadPressed(padNo, (PadButton)m.code)) return true;
				break;
			case InputType::GamepadAxis:
			{
				float axisVal = GetAxisValue(padNo, (PadAxis)m.code);
				if (axisVal * m.scale > AXIS_THRESHOLD || axisVal * m.scale < -AXIS_THRESHOLD) return true;
				break;
			}
			case InputType::Mouse:
				if (IsMousePressed((MouseButton)m.code)) return true;
				break;
		}
	}
	return false;
}

bool InputManager::IsActionTriggered(ActionID action, int padNo) const
{
	const auto& mappings = KeyConfig::GetInstance().GetMappings(action);
	for (const auto& m : mappings)
	{
		switch (m.inputType)
		{
			case InputType::Keyboard:
				if (IsKeyTriggered(m.code)) return true;
				break;
			case InputType::GamepadButton:
				if (IsPadTriggered(padNo, (PadButton)m.code)) return true;
				break;
			case InputType::GamepadAxis:
				break; // 連続値のため、実装しない
			case InputType::Mouse:
				if (IsMouseTriggered((MouseButton)m.code)) return true;
				break;
		}
	}
	return false;
}

bool InputManager::IsActionReleased(ActionID action, int padNo) const
{
	const auto& mappings = KeyConfig::GetInstance().GetMappings(action);
	for (const auto& m : mappings)
	{
		switch (m.inputType)
		{
			case InputType::Keyboard:
				if (IsKeyReleased(m.code)) return true;
				break;
			case InputType::GamepadButton:
				if (IsPadReleased(padNo, (PadButton)m.code)) return true;
				break;
			case InputType::GamepadAxis:
				break; // 連続値のため、実装しない
			case InputType::Mouse:
				if (IsMouseReleased((MouseButton)m.code)) return true;
				break;
		}
	}
	return false;
}

void InputManager::SetTriggerCallback(ActionID action, ActionCallback callback)
{
	triggerCallbacks[action] = callback;
}

void InputManager::ClearTriggerCallbacks()
{
	triggerCallbacks.clear();
}

void InputManager::SetPressCallback(ActionID action, ActionCallback callback)
{
	pressCallbacks[action] = callback;
}

void InputManager::ClearPressCallbacks()
{
	pressCallbacks.clear();
}

void InputManager::SetReleaseCallback(ActionID action, ActionCallback callback)
{
	releaseCallbacks[action] = callback;
}

void InputManager::ClearReleaseCallbacks()
{
	releaseCallbacks.clear();
}

void InputManager::DispatchCallbacks()
{
	for (const auto& [action, callback] : triggerCallbacks)
	{
		if (IsActionTriggered(action)) callback();
	}
	for (const auto& [action, callback] : pressCallbacks)
	{
		if (IsActionPressed(action)) callback();
	}
	for (const auto& [action, callback] : releaseCallbacks)
	{
		if (IsActionReleased(action)) callback();
	}
}

bool InputManager::GetRawState(int padNo, PadButton btn)
{
	int dxNo = padNo + 1;
	int type = GetJoypadType(dxNo);
	DINPUT_JOYSTATE dState;
	XINPUT_STATE xState;
	GetJoypadDirectInputState(dxNo, &dState);
	GetJoypadXInputState(dxNo, &xState);

	switch (btn)
	{
		case PadButton::Pad_Face_Left: // Xbox:X, PS:□
			if (type == DX_PADTYPE_DUAL_SENSE) return dState.Buttons[0] != 0;
			return (dState.Buttons[2] != 0) || (xState.Buttons[XINPUT_BUTTON_X] != 0);

		case PadButton::Pad_Face_Right: // Xbox:B, PS:〇
			if (type == DX_PADTYPE_DUAL_SENSE) return dState.Buttons[2] != 0;
			return (dState.Buttons[1] != 0) || (xState.Buttons[XINPUT_BUTTON_B] != 0);

		case PadButton::Pad_Face_Top: // Xbox:Y, PS:△
			return (dState.Buttons[3] != 0) || (xState.Buttons[XINPUT_BUTTON_Y] != 0);

		case PadButton::Pad_Face_Down: // Xbox:A, PS:×
			if (type == DX_PADTYPE_DUAL_SENSE) return dState.Buttons[1] != 0;
			return (dState.Buttons[0] != 0) || (xState.Buttons[XINPUT_BUTTON_A] != 0);

		case PadButton::Pad_Left:
			return (dState.POV[0] == 27000) || (xState.Buttons[XINPUT_BUTTON_DPAD_LEFT] != 0);
		case PadButton::Pad_Right:
			return (dState.POV[0] == 9000) || (xState.Buttons[XINPUT_BUTTON_DPAD_RIGHT] != 0);
		case PadButton::Pad_Top:
			return (dState.POV[0] == 0) || (xState.Buttons[XINPUT_BUTTON_DPAD_UP] != 0);
		case PadButton::Pad_Down:
			return (dState.POV[0] == 18000) || (xState.Buttons[XINPUT_BUTTON_DPAD_DOWN] != 0);

		case PadButton::Pad_L_Shoulder: return (dState.Buttons[4] != 0) || (xState.Buttons[XINPUT_BUTTON_LEFT_SHOULDER] != 0);
		case PadButton::Pad_R_Shoulder: return (dState.Buttons[5] != 0) || (xState.Buttons[XINPUT_BUTTON_RIGHT_SHOULDER] != 0);

		case PadButton::Pad_L_Trigger:
			// XInputは0-255の値が来るので、30以上を「押し込み」と判定
			if (type == DX_PADTYPE_XBOX_360 || type == DX_PADTYPE_XBOX_ONE) return xState.LeftTrigger > 30;
			return (dState.Buttons[6] != 0);
		case PadButton::Pad_R_Trigger:
			if (type == DX_PADTYPE_XBOX_360 || type == DX_PADTYPE_XBOX_ONE) return xState.RightTrigger > 30;
			return (dState.Buttons[7] != 0);
		case PadButton::Pad_L_Thumb: return (dState.Buttons[8] != 0) || (xState.Buttons[XINPUT_BUTTON_LEFT_THUMB] != 0);
		case PadButton::Pad_R_Thumb: return (dState.Buttons[9] != 0) || (xState.Buttons[XINPUT_BUTTON_RIGHT_THUMB] != 0);
		case PadButton::Xbox_Start:
			return (dState.Buttons[10] != 0) || (xState.Buttons[XINPUT_BUTTON_START] != 0);
		case PadButton::Xbox_back:
			return (dState.Buttons[11] != 0) || (xState.Buttons[XINPUT_BUTTON_BACK] != 0);
		default: return false;
	}
}