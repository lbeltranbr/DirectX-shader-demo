#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;
	m_GameInput.rclick		= false;
	m_GameInput.lclick		= false;
	m_GameInput.x_pos = 0;
	m_GameInput.y_pos = 0;
	m_GameInput.m_pitch = 0;
	m_GameInput.m_yaw = 0;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;
	
	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward	 = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;

	//space
	if (kb.Space) m_GameInput.generate = true;
	else		m_GameInput.generate = false;
	 
	//Z key
	if (kb.Z) m_GameInput.smooth = true;
	else		m_GameInput.smooth = false;

	if (mouse.leftButton) m_GameInput.lclick = true;
	else m_GameInput.lclick = false;

	if (mouse.rightButton) m_GameInput.rclick = true;
	else m_GameInput.rclick = false;

	//m_mouse->SetMode(mouse.leftButton ? DirectX::Mouse::MODE_RELATIVE : DirectX::Mouse::MODE_ABSOLUTE);

	m_GameInput.x_pos = mouse.x;
	m_GameInput.y_pos = mouse.y;
	

}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
