// CommDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DMProg.h"
#include "CommDialog.h"


// CCommDialog dialog

IMPLEMENT_DYNAMIC(CCommDialog, CDialog)

CCommDialog::CCommDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCommDialog::IDD, pParent)
	, m_nRadioBtn(0)
{

}

CCommDialog::~CCommDialog()
{
}

int nBtnIDs[]=
{
	IDC_RADIO2,			/* COM1 */
	IDC_RADIO3,			/* COM2 */
	IDC_RADIO4,			/* COM3 */
	IDC_RADIO5,			/* COM4 */
	IDC_RADIO6,			/* COM5 */
	IDC_RADIO7,			/* COM6 */
	IDC_RADIO8,			/* COM7 */
	IDC_RADIO9,			/* COM8 */
	IDC_RADIO10,		/* COM9 */
	IDC_RADIO11,		/* COM10 */
	IDC_RADIO12,		/* COM11 */
	IDC_RADIO13,		/* COM12 */
	IDC_RADIO14,		/* COM13 */
	IDC_RADIO15,		/* COM14 */
	IDC_RADIO16,		/* COM15 */
	IDC_RADIO17			/* COM15 */
};

void CCommDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO2, m_nRadioBtn);
}

BOOL CCommDialog::OnInitDialog()
{
	int				i,id,valid=-1,first=-1;
	HANDLE			hComm;
	CString			csPort;
	CButton			*pBtn;

	CDialog::OnInitDialog();

	if (m_nRadioBtn > 15)  m_nRadioBtn = 15;
	if (m_nRadioBtn < 0)  m_nRadioBtn = 0;

	for (i=0;i<16;i++)
	{
		csPort.Format(_T("COM%d"), i+1);
		hComm = CreateFile(csPort.GetBuffer(0),			// communication port string (COMX)
					     GENERIC_READ | GENERIC_WRITE,	// read/write types
					     0,								// comm devices must be opened with exclusive access
					     NULL,							// no security attributes
					     OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
					     FILE_FLAG_OVERLAPPED,			// Async I/O
					     0);							// template must be 0 for comm devices

		id = nBtnIDs[i];
		pBtn = (CButton *)GetDlgItem(id);	
		if (pBtn) pBtn->EnableWindow(hComm != INVALID_HANDLE_VALUE);
		if (hComm != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hComm);
			if (first<0)first=i;
			if (i==m_nRadioBtn)valid=i;
		}

		hComm = NULL;
	}

	if (valid>=0)m_nRadioBtn=valid;
	else 
		if (first >=0) m_nRadioBtn=first;

	CDataExchange *pDX = (CDataExchange*)new BYTE[sizeof(CDataExchange)];
	pDX->m_bSaveAndValidate = false;
	pDX->m_pDlgWnd = this;
	DDX_Radio(pDX, IDC_RADIO2, m_nRadioBtn); //включает третий RB
	delete pDX; 

	return TRUE;
}

BEGIN_MESSAGE_MAP(CCommDialog, CDialog)
END_MESSAGE_MAP()


// CCommDialog message handlers
