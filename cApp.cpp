#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{

}

cApp::~cApp()
{

}

bool cApp::OnInit()
{
	m_frame = new HoneyPot::cMain();
	m_frame->SetIcon(wxICON(frame_icon));
	m_frame->Show();
	return true;
}