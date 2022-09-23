#pragma once

#include "wx/wx.h"
#include "cMain.h"
#include "resource.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();

private:
	HoneyPot::cMain* m_frame = nullptr;

public:
	virtual bool OnInit();
};

