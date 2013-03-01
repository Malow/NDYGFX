#ifndef KEYLISTENER_H
#define KEYLISTENER_H

#include "DirectX.h"
#include "Process.h"
#include "iKeyListener.h"
#include "CursorControl.h"

/*
This class can either be set a notifier to which it will send keypresses / releases as events.
Or it can be inherited and implemented with the virtual functions 
virtual void KeyPressed(char key) { };
virtual void KeyReleased(char key) { };
In that case it needs to be set as the keylistener in the MaloWEngineParams
*/


class KeyPressedEvent : public MaloW::ProcessEvent
{
private:
	WPARAM key;
	bool pressed;

public:
	KeyPressedEvent(WPARAM key, bool pressed) { this->key = key; this->pressed = pressed; }
	virtual ~KeyPressedEvent() { }
	WPARAM GetKey() const { return this->key; }
	bool GetPressed() const { return this->pressed; }
};

class MouseClickEvent : public MaloW::ProcessEvent
{
private:
	int button;
	bool pressed;

public:
	MouseClickEvent(int button, bool pressed) { this->button = button; this->pressed = pressed; }
	virtual ~MouseClickEvent() {}
	int GetButton() const { return this->button; }
	bool GetPressed() const { return this->pressed; }
};

	

class KeyListener : public iKeyListener
{
private:
	bool keys[256];
	bool mouse[50]; // Support 50 buttons
	MaloW::Process* notifier;
	HWND hwnd;

public:
	KeyListener(HWND handle, MaloW::Process* notifier = NULL);
	virtual ~KeyListener() { }

	virtual void KeyDown(WPARAM param);
	virtual void KeyUp(WPARAM param);

	virtual void MouseDown(int button);
	virtual void MouseUp(int button);

	virtual bool IsPressed(char key);
	virtual bool IsPressed(int key);
	virtual bool IsClicked(int button);
	//bool HasBeenPressedSinceLast(char NotYetImplemented) { }

	/*! Returns the mouse-position relative to the window (excluding borders) */
	virtual Vector2 GetMousePosition() const;

	/*! Sets the mouse-position relative to the window (excluding borders) */
	virtual void SetMousePosition(const Vector2& mousePos);

	virtual void SetCursorVisibility(bool visi);

	void SetHWND(HWND hWnd) { this->hwnd = hWnd; }
};


#endif