
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();

	void SetStatusText(CString csParam);
	void SetStatusProcess(CString csParam);
	void SetEnableToolbar(BOOL bOn);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar			m_wndToolBar;
	CStatusBar			m_wndStatusBar;
	CString				m_csText;
	CString				m_csProcess;
	BOOL					m_bOn;
	BOOL					m_bUpdateAccess;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdatePageText(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePageProcess(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHelpUpdate(CCmdUI *pCmdUI);


	DECLARE_MESSAGE_MAP()

};


