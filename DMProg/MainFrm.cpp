
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DMProg.h"

#include "MainFrm.h"

#include "DMProgParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_STATUS_TEXT, OnUpdatePageText)
	ON_UPDATE_COMMAND_UI(ID_STATUS_PROCESS, OnUpdatePageProcess)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_HELP_UPDATE, OnUpdateHelpUpdate)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_STATUS_TEXT,
	ID_STATUS_PROCESS
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWinApp* pApp = AfxGetApp();
	UINT	nID, nStyle;
	INT		nWidth, StatusWidth;
	RECT	rc;

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_bOn = 1;

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	GetWindowRect(&rc);
	StatusWidth = rc.right - rc.left;

	m_csText = _T("");
	m_wndStatusBar.GetPaneInfo(0, nID, nStyle, nWidth);
	nWidth = 290;
	m_wndStatusBar.SetPaneInfo(0, nID, SBPS_NORMAL, nWidth);
	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, nWidth);
	nWidth = 300;
	m_wndStatusBar.SetPaneInfo(1, nID, SBPS_NORMAL, nWidth);
	


	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	m_bUpdateAccess = pApp->GetProfileInt( VERSION_NAME, PARAM_UPDATE_ACCESS, 0);
	pApp->WriteProfileInt(VERSION_NAME, PARAM_UPDATE_ACCESS, m_bUpdateAccess);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


void CMainFrame::OnUpdatePageText(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(); 
	pCmdUI->SetText( m_csText ); 		
}
void CMainFrame::OnUpdatePageProcess(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(); 
	pCmdUI->SetText( m_csProcess ); 
	//pCmdUI->SetText( _T("sûû") ); 
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bOn);
}

void CMainFrame::SetStatusText(CString csParam) 
{
	m_csText = csParam;
};
void CMainFrame::SetStatusProcess(CString csParam) 
{
	m_csProcess = csParam;
};
void CMainFrame::SetEnableToolbar(BOOL bOn) 
{
	m_bOn = bOn;
};


void CMainFrame::OnUpdateHelpUpdate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bUpdateAccess);
}


// CMainFrame message handlers
