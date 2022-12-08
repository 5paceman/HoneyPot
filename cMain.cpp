#include "cMain.h"


HoneyPot::cMain::cMain() : wxFrame(nullptr, wxID_ANY, "HoneyPot Injector", wxPoint(30, 30), wxSize(500, 470))
{
	SetIcon(wxICON(frame_icon));
	SetMaxSize(wxSize(500, 470));
	SetMinSize(wxSize(500, 470));
	m_mainPane = new wxPanel(this);
	
	wxStaticText* pathLabel = new wxStaticText(m_mainPane, wxID_ANY, "DLL Path:", wxPoint(9, 5), wxDefaultSize);
	m_filePathTxt = new wxTextCtrl(m_mainPane, wxID_ANY, "", wxPoint(10, 23), wxSize(390, 20));
	
	m_browseButton = new wxButton(m_mainPane, wxID_ANY, "Browse", wxPoint(405, 21), wxDefaultSize);
	m_browseButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnBrowseButtonClicked, this);
	
	wxStaticText* injMethodLabel = new wxStaticText(m_mainPane, wxID_ANY, "Injection Method:", wxPoint(60, 50), wxSize(150, 15), wxALIGN_CENTRE_HORIZONTAL);
	m_injMethodList = new wxComboBox(m_mainPane, wxID_ANY, wxEmptyString, wxPoint(60, 68), wxSize(150, 20), 3, injMethods, wxCB_DROPDOWN);
	m_injMethodList->SetEditable(false);
	m_injMethodList->SetSelection(0);
	
	wxStaticText* reMethodLabel = new wxStaticText(m_mainPane, wxID_ANY, "Execution Method:", wxPoint(500 - 75 - 150, 50), wxSize(150, 15), wxALIGN_CENTRE_HORIZONTAL);
	m_reExecList = new wxComboBox(m_mainPane, wxID_ANY, wxEmptyString, wxPoint(500 - 75 - 150, 68), wxSize(150, 20), 2, reMethods, wxCB_DROPDOWN);
	m_reExecList->SetEditable(false);
	m_reExecList->SetSelection(0);
	
	m_injectButton = new wxButton(m_mainPane, wxID_ANY, "Inject", wxPoint(10, 95), wxSize(150, 40));
	m_injectButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnInjectButtonClicked, this);
	
	m_settingsButton = new wxButton(m_mainPane, wxID_ANY, "Options", wxPoint(325, 95), wxSize(150, 40));
	m_settingsButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnSettingsButtonClicked, this);
	
	m_searchBox = new wxTextCtrl(m_mainPane, wxID_ANY, "Search", wxPoint(10, 140), wxSize(465, 20));
	m_listCtrl = new wxProcessListCtrl(m_mainPane, wxID_ANY, wxPoint(10, 165), wxSize(465, 122), m_searchBox);
	m_listCtrl->Bind(wxEVT_LIST_ITEM_SELECTED, &cMain::OnSelectedItem, this);

	wxStaticText* logLabel = new wxStaticText(m_mainPane, wxID_ANY, "Log:", wxPoint(10, 155 + 122 + 13));
	m_logOutput = new wxTextCtrl(m_mainPane, wxID_ANY, "", wxPoint(10, 155 + 122 + 30), wxSize(465, 90), wxTE_READONLY | wxTE_MULTILINE);
	
	std::function<void(const wchar_t*)> logCallback = [this](const wchar_t* msg) { OnLogOutput(msg); };
	Logger::getInstance().registerLogCallback(logCallback);

	m_refreshButton = new wxButton(m_mainPane, wxID_ANY, "Refresh", wxPoint(500 - 100, 405));
	m_refreshButton->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cMain::OnRefreshButtonClicked, this);
	
	m_selectedPidText = new wxStaticText(m_mainPane, wxID_ANY, "Selected PID: " + selectedPID, wxPoint(10, 410));

	m_cOptions = new cOptions(this);
	RCast<cOptions*>(m_cOptions)->m_tlsCallbacksCheckbox->SetValue(options.mmTLS);
	RCast<cOptions*>(m_cOptions)->m_sehCheckbox->SetValue(options.mmSEH);
	RCast<cOptions*>(m_cOptions)->m_fakeHeadersCheckbox->SetValue(options.mmFH);
	RCast<cOptions*>(m_cOptions)->m_clearHeadersCheckbox->SetValue(options.mmCH);
	RCast<cOptions*>(m_cOptions)->m_cloakThreadCheckbox->SetValue(options.loCT);
}

void HoneyPot::cMain::OnLogOutput(const wchar_t* message)
{
	std::ostream stream(m_logOutput);
	stream << wxString(message) << "\n";
	stream.flush();
}

void HoneyPot::cMain::OnBrowseButtonClicked(wxCommandEvent& evt)
{
	if (m_fileDialog == nullptr)
	{
		m_fileDialog = new wxFileDialog(this, _("Open DLL file"), "", "", "DLL files (*.dll)|*.dll", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	}

	if (m_fileDialog->ShowModal() == wxID_OK)
	{
		m_filePathTxt->SetValue(m_fileDialog->GetPath());
	}
}

void HoneyPot::cMain::OnSearchBoxType(wxKeyEvent& evt)
{
	if (m_searchBox->GetValue().compare(L"Search") == 0)
	{
		m_searchBox->SetValue("");
	}
	evt.Skip();
}

void HoneyPot::cMain::OnInjectButtonClicked(wxCommandEvent& evt)
{
	if (selectedPID == 0)
	{
		wxMessageBox(wxT("Select a process."));
		return;
	}


	InjectionData injData{0};
	
	size_t len = (wcslen(m_filePathTxt->GetValue().wchar_str()) + 1) * sizeof(wchar_t);
	memcpy(injData.dllPath, m_filePathTxt->GetValue().wchar_str(), len);

	if (options.loCT)
		injData.flags |= CLOAK_THREAD;
	if (options.mmCH)
		injData.flags |= MM_CLEAR_PE;
	if (options.mmFH)
		injData.flags |= MM_FAKE_PE;
	if (options.mmSEH)
		injData.flags |= MM_SEH;
	if (options.mmTLS)
		injData.flags |= MM_TLS;

	injData.pID = selectedPID;
	injData.injMethod = InjMethod::IM_NONE;
	injData.reMethod = RemoteExecMethod::RE_NONE;

	if (GetInjectionMethods(injData)) {
		nightshade::Injection inj(&injData);
		if (inj.Inject()) {
			wxMessageBox(wxString(L"Success"));
		}
		else {
			wxMessageBox(wxString(L"Failed to inject, check Logs"));
		}
	}
}

void HoneyPot::cMain::OnSettingsButtonClicked(wxCommandEvent& evt)
{
	if (m_cOptions->ShowModal() == wxID_OK)
	{
		options.mmTLS = RCast<cOptions*>(m_cOptions)->m_tlsCallbacksCheckbox->IsChecked();
		options.mmSEH = RCast<cOptions*>(m_cOptions)->m_sehCheckbox->IsChecked();
		options.mmFH = RCast<cOptions*>(m_cOptions)->m_fakeHeadersCheckbox->IsChecked();
		options.mmCH = RCast<cOptions*>(m_cOptions)->m_clearHeadersCheckbox->IsChecked();
		options.loCT = RCast<cOptions*>(m_cOptions)->m_cloakThreadCheckbox->IsChecked();
	}
}

void HoneyPot::cMain::OnRefreshButtonClicked(wxCommandEvent& evt)
{
	m_listCtrl->RefreshProcesses();
	selectedPID = 0;
	m_selectedPidText->SetLabel("Selected PID: " + std::to_wstring(selectedPID));
}

void HoneyPot::cMain::OnSelectedItem(wxListEvent& evt)
{
	selectedPID = m_listCtrl->GetSelectedProcessID();
	m_selectedPidText->SetLabel("Selected PID: " + std::to_wstring(selectedPID));
}

bool HoneyPot::cMain::GetInjectionMethods(InjectionData& data)
{
	wxString method = m_injMethodList->GetStringSelection();
	wxString reMethod = m_reExecList->GetStringSelection();
	if (method.empty() || reMethod.empty())
	{
		wxMessageBox(wxT("Select an Injection Method."));
		return false;
	}

	bool foundInjMethod = false;
	if (method.compare(L"LoadLibrary") == 0)
	{
		data.injMethod = InjMethod::IM_LoadLibraryEx;
		foundInjMethod = true;
	}
	else if (method.compare(L"LdrLoadDll") == 0)
	{
		data.injMethod = InjMethod::IM_LdrLoadDll;
		foundInjMethod = true;
	}
	else if (method.compare(L"ManualMap") == 0)
	{
		data.injMethod = InjMethod::IM_ManualMap;
		foundInjMethod = true;
	}

	bool foundReMethod = false;
	if (reMethod.compare(L"NtCreateThreadEx") == 0)
	{
		data.reMethod = RemoteExecMethod::RE_NtCreateThreadEx;
		foundReMethod = true;
	}
	else if (reMethod.compare(L"QueueUserAPC") == 0)
	{
		data.reMethod = RemoteExecMethod::RE_QueueUserAPC;
		foundReMethod = true;
	}
	else if (reMethod.compare(L"ThreadHijacking") == 0)
	{
		data.reMethod = RemoteExecMethod::RE_HijackThread;
		foundReMethod = true;
	}

	return foundInjMethod && foundReMethod;
}




