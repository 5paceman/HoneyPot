#include "wxProcessListCtrl.h"
#include "../nightshade/Utils.h"

wxProcessListCtrl::wxProcessListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, wxTextCtrl* searchBox) : wxListCtrl(parent, id, pos, size, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_VIRTUAL)
{
	m_imageList = new wxImageList(16, 16);
	m_searchBox = searchBox;
	HICON hDefApp;
	HRESULT result = LoadIconWithScaleDown(NULL, MAKEINTRESOURCE(IDI_APPLICATION), 16, 16, &hDefApp);
	if (result != S_OK)
		LOG(1, L"Unable to load default app icon.");

	m_defaultIcon.CreateFromHICON(WXHICON(hDefApp));

	RefreshProcesses();
	if (m_searchBox != nullptr)
	{
		m_searchBox->Bind(wxEVT_KEY_DOWN, &wxProcessListCtrl::OnSearchBoxType, this);
		m_searchBox->Bind(wxEVT_KEY_UP, &wxProcessListCtrl::OnSearchBoxType, this);
	}

}

unsigned long wxProcessListCtrl::GetSelectedProcessID()
{
	const std::vector<ProcessData>* list;
	if (m_searchBox->GetValue().IsEmpty() || m_searchBox->GetValue().compare(L"Search") == 0)
	{
		list = &m_activeProcessList;
	}
	else {
		list = &m_shownProcessList;
	}

	long itemId = this->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if (itemId > list->size())
		return 0;

	
	wxString pidStr = OnGetItemText(itemId, 1);
	unsigned long pid = 0;
	pidStr.ToCULong(&pid, 10);
	return pid;
}

int wxProcessListCtrl::OnGetItemImage(long item) const
{
	const std::vector<ProcessData>* list;
	if (m_searchBox->GetValue().IsEmpty() || m_searchBox->GetValue().compare(L"Search") == 0)
	{
		list = &m_activeProcessList;
	}
	else {
		list = &m_shownProcessList;
	}

	return list->at(item).hasIcon ? list->at(item).iconIndex : m_defaultIconIndex;
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

	if (item > list->size() || column > 3)
		return wxString();
	
	wxString string;
	switch (column)
	{
	case 1:
		string << list->at(item).processID;
		break;
	case 2:
		string = list->at(item).windowName;
		break;
	case 3:
		string = list->at(item).szExeFile;
	}
	
	return string;
}

void wxProcessListCtrl::RefreshProcesses()
{
	if (m_searchBox != nullptr)
		m_searchBox->Clear();

	if (m_imageList != nullptr)
	{
		m_imageList->Destroy();
		m_imageList->Create(16, 16);
		m_defaultIconIndex = m_imageList->Add(m_defaultIcon);
	}

	this->ClearAll();

	int iconWidth = 30;
	int pidWidth = 60;
	int windowWidth = this->GetSize().GetWidth() / 2 - pidWidth;
	int processWidth = this->GetSize().GetWidth() / 2 - pidWidth;
	this->InsertColumn(0, _(""), wxLIST_FORMAT_CENTER, iconWidth);
	this->InsertColumn(1, _("PID"), wxLIST_FORMAT_LEFT, pidWidth);
	this->InsertColumn(2, _("Window Name"), wxLIST_FORMAT_LEFT, windowWidth);
	this->InsertColumn(3, _("Process"), wxLIST_FORMAT_LEFT, processWidth);

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
			procData.windowName = nightshade::Utils::GetWindowTitleFromPID(procEntry.th32ProcessID);
			procData.szExeFile = procEntry.szExeFile;
			

			if (wxString(procEntry.szExeFile).EndsWith(".exe"))
			{
				HICON hIcon = 0;
				hIcon = nightshade::Utils::GetProcessIcon(procData.processID);

				if (hIcon != NULL)
				{
					wxIcon wIcon;
					wIcon.CreateFromHICON(WXHICON(hIcon));
					wIcon = wIcon;
					procData.iconIndex = m_imageList->Add(wIcon);
					procData.hasIcon = true;
					DestroyIcon(hIcon);
				}
				
			}

			m_activeProcessList.push_back(procData);
		} while (Process32Next(hSnapshot, &procEntry));
		CloseHandle(hSnapshot);
	}
	this->SetItemCount(m_activeProcessList.size());
	this->SetImageList(m_imageList, wxIMAGE_LIST_SMALL);
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
