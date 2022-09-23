#include "wxProcessListCtrl.h"
#include "../nightshade/Utils.h"

wxProcessListCtrl::wxProcessListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, wxTextCtrl* searchBox) : wxListCtrl(parent, id, pos, size, wxLC_REPORT | wxLC_VRULES | wxLC_SINGLE_SEL | wxLC_VIRTUAL)
{
	this->InsertColumn(0, _("PID"), wxLIST_FORMAT_LEFT, 40);
	this->InsertColumn(1, _("Window Name"), wxLIST_FORMAT_LEFT, 197);
	this->InsertColumn(2, _("Process"), wxLIST_FORMAT_LEFT);
	RefreshProcesses();
	m_searchBox = searchBox;
	if (m_searchBox != nullptr)
	{
		m_searchBox->Bind(wxEVT_KEY_DOWN, &wxProcessListCtrl::OnSearchBoxType, this);
		m_searchBox->Bind(wxEVT_KEY_UP, &wxProcessListCtrl::OnSearchBoxType, this);
	}
}

unsigned long wxProcessListCtrl::GetSelectedProcessID()
{
	long itemId = this->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (itemId > m_shownProcessList.size())
		return 0;

	
	wxString pidStr = OnGetItemText(itemId, 0);
	unsigned long pid = 0;
	pidStr.ToCULong(&pid, 10);
	return pid;
}

wxString wxProcessListCtrl::OnGetItemText(long item, long column) const
{
	const std::vector<ProcessData>* list;
	if (m_searchBox->GetValue().IsEmpty() || m_searchBox->GetValue().compare(L"Search") == 0)
	{
		list = &m_activeProcessList;
	}
	else {
		list = &m_shownProcessList;
	}

	if (item > list->size() || column > 2)
		return wxString();
	
	wxString string;
	switch (column)
	{
	case 0:
		string << list->at(item).processID;
		break;
	case 1:
		string = list->at(item).windowName;
		break;
	case 2:
		string = list->at(item).szExeFile;
	}
	
	return string;
}

void wxProcessListCtrl::RefreshProcesses()
{
	if (m_searchBox != nullptr)
		m_searchBox->Clear();

	m_activeProcessList.clear();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hSnapshot, &procEntry))
		{
			return;
		}
		do {
			ProcessData procData{ 0 };
			procData.processID = procEntry.th32ProcessID;
			procData.windowName = nightshade::GetWindowTitleFromPID(procEntry.th32ProcessID);
			procData.szExeFile = procEntry.szExeFile;
			m_activeProcessList.push_back(procData);
		} while (Process32Next(hSnapshot, &procEntry));
		CloseHandle(hSnapshot);
	}
	this->SetItemCount(m_activeProcessList.size());
	this->Refresh();
}

void wxProcessListCtrl::OnSearchBoxType(wxKeyEvent& evt)
{
	if (evt.GetEventType() == wxEVT_KEY_DOWN)
	{
		if (wcscmp(m_searchBox->GetValue().c_str(), L"Search") == 0)
		{
			m_searchBox->Clear();
		}
	}
	else {
		m_shownProcessList.clear();
		for (const ProcessData procData : m_activeProcessList)
		{
			if (wxString(procData.szExeFile).Lower().starts_with(m_searchBox->GetValue().Lower()))
			{
				m_shownProcessList.push_back(procData);
			}
		}
		this->SetItemCount(m_shownProcessList.size());
		this->Refresh();
	}
	evt.Skip();
}
