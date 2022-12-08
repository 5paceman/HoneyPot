#pragma once

#include <wx/wx.h>
#include "cMain.h"

namespace HoneyPot {

	class cOptions : public wxDialog
	{
	public:
		cOptions(wxWindow* parent);

	public:
		wxPanel* m_mainPane = nullptr;

		wxStaticBox* m_manualMapBox = nullptr;

		wxCheckBox* m_sehCheckbox = nullptr;
		wxCheckBox* m_clearHeadersCheckbox = nullptr;
		wxCheckBox* m_fakeHeadersCheckbox = nullptr;
		wxCheckBox* m_tlsCallbacksCheckbox = nullptr;
		wxCheckBox* m_cloakThreadCheckbox = nullptr;

	public:
		void OnSEHCheckboxClick(wxCommandEvent& evt);
		void OnClearHeadersCheckboxClick(wxCommandEvent& evt);
		void OnFakeHeadersCheckboxClick(wxCommandEvent& evt);
		void OnTLSCallbackCheckboxClick(wxCommandEvent& evt);
		void OnCloakThreadCheckboxClick(wxCommandEvent& evt);

		void OnClose(wxCloseEvent& evt);

		DECLARE_EVENT_TABLE()
	};
}