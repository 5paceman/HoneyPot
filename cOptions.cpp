#include "cOptions.h"

BEGIN_EVENT_TABLE(HoneyPot::cOptions, wxDialog)
	EVT_CLOSE(HoneyPot::cOptions::OnClose)
END_EVENT_TABLE()

HoneyPot::cOptions::cOptions(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Options", wxPoint(30, 30), wxSize(400, 200))
{
	SetMaxSize(wxSize(400, 500));
	SetMinSize(wxSize(400, 500));
	m_mainPane = new wxPanel(this);


	m_manualMapBox = new wxStaticBox(m_mainPane, wxID_ANY, "Manual Mapping", wxPoint(10, 10), wxSize(350, 70));

	m_sehCheckbox = new wxCheckBox(m_manualMapBox, wxID_ANY, "Exception Handling (SEH)", wxPoint(25, 20));

	m_tlsCallbacksCheckbox = new wxCheckBox(m_manualMapBox, wxID_ANY, "TLS Callbacks", wxPoint(25, 40));

	m_clearHeadersCheckbox = new wxCheckBox(m_manualMapBox, wxID_ANY, "Clear Headers", wxPoint(220, 20));

	m_fakeHeadersCheckbox = new wxCheckBox(m_manualMapBox, wxID_ANY, "Fake Headers", wxPoint(220, 40));

	wxStaticBox* m_launchOptionsBox = new wxStaticBox(m_mainPane, wxID_ANY, "Launch Options", wxPoint(10, 90), wxSize(350, 50));

	m_cloakThreadCheckbox = new wxCheckBox(m_launchOptionsBox, wxID_ANY, "Cloak Thread", wxPoint(25, 20));
}

void HoneyPot::cOptions::OnClose(wxCloseEvent& event)
{
	wxDialog::EndModal(wxID_OK);
}

