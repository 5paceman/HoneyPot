#pragma once

#include <wx/wx.h>
#include "cMain.h"

namespace HoneyPot {
	class cOptions : public wxFrame
	{
	public:
		cOptions(cMain* mainFrame);

	private:
		cMain* mainFrame = nullptr;

		wxCheckBox* sehCheckbox = nullptr;
		wxCheckBox
	};
}