#pragma once
#include "wx/wx.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <wx/listctrl.h>

struct ProcessData {
	unsigned long processID;
	std::wstring szExeFile;
	std::wstring windowName;
};

class wxProcessListCtrl : public wxListCtrl
{
public:
	wxProcessListCtrl(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, wxTextCtrl* searchBox = nullptr);

public:
	unsigned long GetSelectedProcessID();
	void RefreshProcesses();
	void OnSearchBoxType(wxKeyEvent& evt);

private:
	virtual wxString OnGetItemText(long item, long column) const wxOVERRIDE;

private:
	unsigned long m_selectedProcessID = 0;
	std::vector<ProcessData> m_activeProcessList;
	std::vector<ProcessData> m_shownProcessList;
	wxTextCtrl* m_searchBox = nullptr;
};