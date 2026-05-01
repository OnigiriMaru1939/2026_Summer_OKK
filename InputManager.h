#pragma once
#include <string>
#include <functional>
#include <map>
#include "ActionID.h"

enum class PadAxis { Pad_L_X, Pad_L_Y, Pad_R_X, Pad_R_Y};
// ゲームコントローラーの認識番号
// DxLib定数、DX_INPUT_PAD1等に対応
enum class JOYPAD_NO
{
	KEY_PAD1 = 0,			// キー入力とパッド１入力
	PAD1 = 1,				// パッド１入力
	PAD2 = 2,				// パッド２入力
	PAD3 = 3,				// パッド３入力
	PAD4 = 4,				// パッド４入力
	INPUT_KEY = 4096	// キー入力
};

// ゲームコントローラータイプ
// DxLib定数、DX_OTHER等に対応
enum class JOYPAD_TYPE
{
	OTHER = 0,
	XBOX_360,
	XBOX_ONE,
	DUAL_SHOCK_4,
	DUAL_SENSE,
	SWITCH_JOY_CON_L,
	SWITCH_JOY_CON_R,
	SWITCH_PRO_CTRL,
	MAX
};

// マウスボタンの種類
enum class MouseButton
{
	Mouse_Left = 0,
	Mouse_Right,
	Mouse_Middle
};

// コントローラーのボタンの種類
enum class PadButton
{
	Pad_Face_Left = 0,
	Pad_Face_Right,
	Pad_Face_Top,
	Pad_Face_Down,
	Pad_Left,
	Pad_Right,
	Pad_Top,
	Pad_Down,
	Pad_L_Shoulder,
	Pad_R_Shoulder,
	Pad_L_Trigger,
	Pad_R_Trigger,
	Pad_L_Thumb,
	Pad_R_Thumb,
	Xbox_Start,// Xbox:Start
	Xbox_back, // Xbox:Back
	Max
};

class InputManager
{
private:
	InputManager();
	~InputManager();
	static constexpr int KEY_COUNT = 256;
	static constexpr int MOUSE_BUTTON_COUNT = 3;
	static constexpr int MAX_JOYPADS = 4;
	static constexpr int DEADZONE = 50; // スティックのデッドゾーン
	static constexpr float STICK_MAX = 1000.0f; // スティックの最大値
	static constexpr float AXIS_THRESHOLD = 0.5f; // 軸入力を「押された」とみなす閾値

public:
	// シングルトン
	static InputManager& GetInstance()
	{
		static InputManager instance;
		return instance;
	}

	void Update();
	void DrawDebug(int x, int y) const;

	// キー入力
	bool IsKeyPressed(int keyCode) const;
	bool IsKeyTriggered(int keyCode) const;
	bool IsKeyReleased(int keyCode) const;

	// マウス入力
	bool IsMousePressed(MouseButton button) const;
	bool IsMouseTriggered(MouseButton button) const;
	bool IsMouseReleased(MouseButton button) const;
	int GetMouseX() const { return mouseX; }
	int GetMouseY() const { return mouseY; }
	int GetMouseDX() const { return mouseX - prevMouseX; }
	int GetMouseDY() const { return mouseY - prevMouseY; }

	// パッド入力
	bool IsPadPressed(int no, PadButton btn) const;
	bool IsPadTriggered(int no, PadButton btn) const;
	bool IsPadReleased(int no, PadButton btn) const;
	// スティック値取得 (-1000 ~ 1000)
	int GetPadStickLX(int padIndex) const { return padLX[padIndex]; }
	int GetPadStickLY(int padIndex) const { return padLY[padIndex]; }
	int GetPadStickRX(int padIndex) const { return padRX[padIndex]; }
	int GetPadStickRY(int padIndex) const { return padRY[padIndex]; }
	float GetAxisValue(int padNo, PadAxis axis) const;

	// アクションに対する入力状態を取得する関数
	float GetActionValue(ActionID action, int padNo = 0) const;
	bool IsActionPressed(ActionID action, int padNo = 0) const;
	bool IsActionTriggered(ActionID action, int padNo = 0) const;
	bool IsActionReleased(ActionID action, int padNo = 0) const;

	// アクションIDに対する入力状態を取得する関数
	using ActionCallback = std::function<void()>;

	// アクションに対するコールバックの登録
	void SetTriggerCallback(ActionID action, ActionCallback callback);
	void SetPressCallback(ActionID action, ActionCallback callback);
	void SetReleaseCallback(ActionID action, ActionCallback callback);
	// 登録されたコールバックのクリア
	void ClearTriggerCallbacks();
	void ClearPressCallbacks();
	void ClearReleaseCallbacks();
	// 登録されたコールバックを呼び出す関数
	void DispatchCallbacks();
	// コントローラーごとのボタンマッピングを解決するための関数
	// std::wstring GetActionName(ActionID action) const;

	const char* GetPadName(int type) const;
private:
	std::map<ActionID, ActionCallback> triggerCallbacks;
	std::map<ActionID, ActionCallback> pressCallbacks;
	std::map<ActionID, ActionCallback> releaseCallbacks;
	// キー入力
	int key[KEY_COUNT] = {};
	int prevKey[KEY_COUNT] = {};

	// マウスボタンの入力状態
	int mouseButton[MOUSE_BUTTON_COUNT] = {}; // 0:左 1:右 2:中 
	int prevMouseButton[MOUSE_BUTTON_COUNT] = {};

	int mouseX = 0;
	int mouseY = 0;
	int prevMouseX = 0;
	int prevMouseY = 0;

	// パッド入力状態
	int padButton[MAX_JOYPADS][(int)PadButton::Max] = {};
	int prevPadButton[MAX_JOYPADS][(int)PadButton::Max] = {};
	int padLX[MAX_JOYPADS] = {};
	int padLY[MAX_JOYPADS] = {};
	int padRX[MAX_JOYPADS] = {};
	int padRY[MAX_JOYPADS] = {};

	// コントローラーごとのボタンマッピングを解決するための関数
	bool GetRawState(int padNo, PadButton btn);
	// 入力状態の判定を行う共通関数
	bool IsInputTriggered(int current, int previous) const { return current == 1; }
	bool IsInputPressed(int current) const { return current > 0; }
	bool IsInputReleased(int current, int previous) const { return current == 0 && previous > 0; }
};
