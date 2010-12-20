#pragma once


// CCommDialog dialog

class CCommDialog : public CDialog
{
	DECLARE_DYNAMIC(CCommDialog)

public:
	CCommDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCommDialog();
	virtual  BOOL OnInitDialog();


// Dialog Data
	enum { IDD = IDD_DIALOG_COMM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nRadioBtn;
};
