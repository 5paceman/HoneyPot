#pragma once

#include "wx/wx.h"
#include <wx/listctrl.h>
#include <Windows.h>
#include <TlHelp32.h>
#include "../nightshade/Injection.h"
#include "wxProcessListCtrl.h"
#include "resource.h"
#include "cOptions.h"


namespace HoneyPot {
	struct HP_Options {
		bool mmSEH = true;
		bool mmTLS = true;
		bool mmCH = false;
		bool mmFH = false;
		bool loCT = false;
	};

	class cMain : public wxFrame
	{
	public:
		cMain();
		
	public:
		wxPanel* m_mainPane = nullptr;
		wxProcessListCtrl* m_listCtrl = nullptr;
		wxFileDialog* m_fileDialog = nullptr;
		wxTextCtrl* m_filePathTxt = nullptr;
		wxTextCtrl* m_logOutput = nullptr;
		wxTextCtrl* m_searchBox = nullptr;
		wxComboBox* m_injMethodList = nullptr;
		wxComboBox* m_reExecList = nullptr;


		wxButton* m_injectButton = nullptr;
		wxButton* m_browseButton = nullptr;
		wxButton* m_settingsButton = nullptr;
		wxButton* m_refreshButton = nullptr;

		wxStaticText* m_selectedPidText = nullptr;

		wxDialog* m_cOptions = nullptr;
		HP_Options options = {};
	private:
		unsigned long selectedPID = 0L;

	public:
		void OnLogOutput(const wchar_t* message);
	public:
		void OnBrowseButtonClicked(wxCommandEvent& evt);
		void OnInjectButtonClicked(wxCommandEvent& evt);
		void OnSettingsButtonClicked(wxCommandEvent& evt);
		void OnRefreshButtonClicked(wxCommandEvent& evt);
		void OnSelectedItem(wxListEvent& evt);
		void OnSearchBoxType(wxKeyEvent& evt);
	
	private:
		bool GetInjectionMethods(InjectionData& data);

	public:
		const wxString injMethods[3] = { _("LoadLibrary"), _("LdrLoadDll"), _("ManualMap")};
		const wxString reMethods[2] = { _("NtCreateThreadEx"), _("QueueUserAPC")};

	};
}

