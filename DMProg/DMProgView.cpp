
// DMProgView.cpp : implementation of the CDMProgView class
//

#include "stdafx.h"
#include "DMProg.h"

#include "DMProgDoc.h"
#include "DMProgView.h"
#include "MainFrm.h"

#include "CommDialog.h"
#include "UpdateFirmWare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString		csAddrTXTypeCombo[]=
{
	_T("Адрес \042получателя\042"),
	_T("Первый байт пакета(RTU)"),
	_T("Первые два байта пакета(ASCII)")
};

CString		csAddrRXTypeCombo[]=
{
	_T("Прием \042своего\042 пакета"),
	_T("Прием от \042отправителя\042"),
	_T("И"),
	_T("ИЛИ"),
	_T("От всех")
};

CString		csSQL[]=
{	
	_T("Низкий уровень"),
	_T("Высокий уровень")
};

CString		csTXMode[]=
{	
	_T("По таймауту последнего байта"),
	_T("По количеству байт"),
	_T("По спецсимволу")
};


int			nBaudRates[]=
{
	300,
	600,
	1200,
	2400,
	4800,
	9600,
	19200,
	38400,
	57600
};

CString			csNoneVisibleChar[]=
{
_T("NUL"),
_T("SOH"),
_T("STX"),
_T("ETX"),
_T("EOT"),
_T("ENQ"),
_T("ACK"),
_T("BEL"),
_T("BS"),
_T("TAB"),
_T("LF"),
_T("VT"),
_T("FF"),
_T("CR"),
_T("SO"),
_T("SI"),
_T("DLE"),
_T("DC1"),
_T("DC2"),
_T("DC3"),
_T("DC4"),
_T("NAK"),
_T("SYN"),
_T("ETB"),
_T("CAN"),
_T("EM"),
_T("SUB"),
_T("ESC"),
_T("FS"),
_T("GS"),
_T("RS"),
_T("US")
};

BYTE SerialCommands_BIN_ON[SERIAL_COMMAND_BINARY_ON_SIZE]=
{
	0x032,
};


#define STATUS_EMPTY				0
#define STATUS_RESET_MODEM			1
#define STATUS_READ_VERSION			2
#define STATUS_READ_BASE			3
#define STATUS_WRITE_BASE			4
#define STATUS_READ_SUCC			5
#define STATUS_WRITE_SUCC			6
#define STATUS_ERR_ATHER_VER		7
#define STATUS_ERR_TIMEOUT			8
#define STATUS_ERR_NOANSWER			9


CString		csStatuses[]=
{
	_T(""),												// Empty
	_T("Выключите и сново включите модем"),				// Reset Modem
	_T("Чтение версии программного обеспечения модема"),// Read version
	_T("Чтение параметров модема"),						// Read params
	_T("Запись параметров модема"),						// Write params
	_T("Чтение завершено успешно"),						// Read successful
	_T("Запись завершена успешна"),						// Write successful
	_T("Ошибка: несоответствие версий"),				// Error: Ather version
	_T("Ошибка: истек таймаут ожидания ответа"),		// Error: Timeout
	_T("Ошибка: модем не отвечает")			// Error: Not answer
};

CString		csProccess[]=
{
	_T("\\"),
	_T("|"),
	_T("/"),
	_T("-")	
};

// CDMProgView

IMPLEMENT_DYNCREATE(CDMProgView, CFormView)

BEGIN_MESSAGE_MAP(CDMProgView, CFormView)
	ON_WM_UPDATEUISTATE()
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT_ADDR_SYSTEM, &CDMProgView::OnEnChangeEditAddrSystem)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDR_SYSTEM, &CDMProgView::OnEnChangeEditAddrSystem)
	ON_EN_CHANGE(IDC_EDIT_ADDR_MODEM, &CDMProgView::OnEnChangeEditAddrModem)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDR_MODEM, &CDMProgView::OnEnChangeEditAddrModem)
	ON_EN_CHANGE(IDC_EDIT_ADDR_SENDER, &CDMProgView::OnEnChangeEditAddrSender)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDR_SENDER, &CDMProgView::OnEnChangeEditAddrSender)
	ON_EN_CHANGE(IDC_EDIT_ADDR_RECIEVER, &CDMProgView::OnEnChangeEditAddrReceiver)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDR_RECIEVER, &CDMProgView::OnEnChangeEditAddrReceiver)

	ON_CBN_SELCHANGE(IDC_COMBO_ADDR_TX, &CDMProgView::OnCbnSelchangeComboAddrTx)
	ON_BN_CLICKED(IDC_BUTTON2, &CDMProgView::OnBnClickedButton2)

	ON_EN_CHANGE(IDC_EDIT_BUFF_SIZE, &CDMProgView::OnEnChangeEditBuffSize)
	ON_EN_KILLFOCUS(IDC_EDIT_BUFF_SIZE, &CDMProgView::OnEnChangeEditBuffSize)
	ON_EN_CHANGE(IDC_EDIT_TIMEOUT_LASTBYTE, &CDMProgView::OnEnChangeEditTimeoutLastbyte)
	
	ON_EN_KILLFOCUS(IDC_EDIT_TIMEOUT_LASTBYTE, &CDMProgView::OnEnKillfocusEditTimeoutLastbyte)
	ON_EN_CHANGE(IDC_EDIT_TO_PRETRANS, &CDMProgView::OnEnChangeEditToPretrans)
	ON_EN_KILLFOCUS(IDC_EDIT_TO_PRETRANS, &CDMProgView::OnEnKillfocusEditToPretrans)
	ON_EN_CHANGE(IDC_EDIT_TO_POSTTRANS, &CDMProgView::OnEnChangeEditToPosttrans)
	ON_EN_KILLFOCUS(IDC_EDIT_TO_POSTTRANS, &CDMProgView::OnEnKillfocusEditToPosttrans)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CDMProgView::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CDMProgView::OnBnClickedButtonWrite)
	ON_WM_TIMER()
	ON_MESSAGE(WM_COMM_RXCHAR, OnRXChar)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDMProgView::OnBnClickedButtonStop)
	ON_CBN_SELCHANGE(IDC_COMBO_ADDR_RX, &CDMProgView::OnCbnSelchangeComboAddrRx)
	ON_CBN_SELCHANGE(IDC_COMBO_SQL, &CDMProgView::OnCbnSelchangeComboSql)
	ON_CBN_SELCHANGE(IDC_COMBO_TXMODE, &CDMProgView::OnCbnSelchangeComboTxmode)
	ON_CBN_SELCHANGE(IDC_COMBO_SPEC_SYM, &CDMProgView::OnCbnSelchangeComboSpecSym)
	ON_BN_CLICKED(IDC_CHECK_LOCAL_ECHO, &CDMProgView::OnBnClickedCheckLocalEcho)
	ON_BN_CLICKED(IDC_CHECK_REMOVE_CONTROL, &CDMProgView::OnBnClickedCheckRemoveControl)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUD, &CDMProgView::OnCbnSelchangeComboBaud)
	ON_BN_CLICKED(IDC_RADIO1, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO2, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO3, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO4, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO5, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO6, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO7, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO8, &CDMProgView::OnBnClickedRadioModem)
	ON_BN_CLICKED(IDC_RADIO9, &CDMProgView::OnBnClickedRadioModem)

	ON_COMMAND(ID_HELP_UPDATE, &CDMProgView::OnHelpUpdate)
END_MESSAGE_MAP()

// CDMProgView construction/destruction

CDMProgView::CDMProgView()
	: CFormView(CDMProgView::IDD)

	, m_nDataBits(0)
	, m_nStopBits(0)
	, m_nParity(0)
	, m_csFirmWarePath(_T(""))
{
	// TODO: add construction code here

}

CDMProgView::~CDMProgView()
{
}

void CDMProgView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_ADDR_TX, m_cbAddrTXType);
	DDX_Control(pDX, IDC_COMBO_ADDR_RX, m_cbAddrRXType);
	DDX_Control(pDX, IDC_COMBO_SQL, m_cbSQL);
	DDX_Control(pDX, IDC_COMBO_TXMODE, m_cbTXMode);
	DDX_Radio(pDX, IDC_RADIO1, m_nDataBits);
	DDX_Control(pDX, IDC_COMBO3, m_cbBaudRates);
	DDX_Radio(pDX, IDC_RADIO5, m_nStopBits);
	DDX_Radio(pDX, IDC_RADIO7, m_nParity);
	DDX_Control(pDX, IDC_COMBO_SPEC_SYM, m_cbSpecSymbol);
	DDX_Control(pDX, IDC_CHECK_REMOVE_CONTROL, m_btRemoutControl);
	DDX_Control(pDX, IDC_CHECK_LOCAL_ECHO, m_btLocalEcho);
}

BOOL CDMProgView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}
//-----------------------------------------------------------------------------
void CDMProgView::InitEditSpin(int nIDEdit, int nIDSpin, int nMin, int nMax, int nStep, int nValue)
{
	TCHAR				szStr[256];
	CEdit				*pEdit;
	CSpinButtonCtrl		*pSpin;
	UDACCEL AccellValue;

	pEdit = (CEdit *)GetDlgItem(nIDEdit);
	pSpin = (CSpinButtonCtrl *)GetDlgItem(nIDSpin);	
	if (pSpin)
	{
		if (pSpin->GetBuddy() != pEdit)
		pSpin->SetBuddy(pEdit);
		pSpin->SetRange(nMin, nMax);
		AccellValue.nSec = 1000;
		AccellValue.nInc = nStep;
		pSpin->SetAccel(1, &AccellValue);
		sprintf_s(szStr, 255,  _T("%d"), nValue);
		if (pEdit) pEdit->SetWindowText(szStr);
	}
}
//-----------------------------------------------------------------------------
void ClearComboBox(CComboBox *pBox)
{
	int i;
	if (pBox)
	for (i = pBox->GetCount()-1; i >= 0; i--)
	{
		pBox->DeleteString( i );
	}
}

void CDMProgView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// TODO:  Add your specialized creation code here
	CDMProgDoc				*pThisDoc;	
	int						nValue;
	int						i, n;
	CString					csTmp;
	CButton					*pBtn;

	LoadSettings();

	csTmp.Format(_T("COM%d"), m_nPort);
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_COMM);	
	if (pBtn) pBtn->SetWindowText(csTmp);

	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	// Address of System
	if (pThisDoc)	nValue=pThisDoc->m_nAddrSystem;
	else	nValue = DEF_PARAM_ADDR_SYSTEM;
	InitEditSpin(IDC_EDIT_ADDR_SYSTEM, IDC_SPIN_ADDR_SYSTEM, 						
								BYTE_RANGE_MIN, BYTE_RANGE_MAX, 1,
								nValue);
	// Address of Modem
	if (pThisDoc)	nValue=pThisDoc->m_nAddrModem;
	else	nValue = DEF_PARAM_ADDR_MODEM;
	InitEditSpin(IDC_EDIT_ADDR_MODEM, IDC_SPIN_ADDR_MODEM, 						
								BYTE_RANGE_MIN, BYTE_RANGE_MAX, 1,
								nValue);
	// Address of Sender
	if (pThisDoc)	nValue=pThisDoc->m_nAddrSender;
	else	nValue = DEF_PARAM_ADDR_SENDER;
	InitEditSpin(IDC_EDIT_ADDR_SENDER, IDC_SPIN_ADDR_SENDER, 						
								BYTE_RANGE_MIN, BYTE_RANGE_MAX, 1,
								nValue);

	// Address of Receiver
	if (pThisDoc)	nValue=pThisDoc->m_nAddrReceiver;
	else	nValue = DEF_PARAM_ADDR_RECEIVER;
	InitEditSpin(IDC_EDIT_ADDR_RECIEVER, IDC_SPIN_ADDR_RECIEVER, 						
								BYTE_RANGE_MIN, BYTE_RANGE_MAX, 1,
								nValue);

	// Address tx
	if (pThisDoc)	nValue=pThisDoc->m_nAddrTXType;
	else	nValue = DEF_PARAM_ADDR_TX;

	m_cbAddrTXType.Clear();
	ClearComboBox(&m_cbAddrTXType);
	n = sizeof(csAddrTXTypeCombo)/sizeof(csAddrTXTypeCombo[0]);
	for (i=0; i<n;i++)
	{
		m_cbAddrTXType.AddString(csAddrTXTypeCombo[i]);
		m_cbAddrTXType.SetItemData(i, i);
	}
	m_cbAddrTXType.SetCurSel(nValue);

	// Address rx
	if (pThisDoc)	nValue=pThisDoc->m_nAddrRXType;
	else	nValue = DEF_PARAM_ADDR_RX;

	m_cbAddrRXType.Clear();
	ClearComboBox(&m_cbAddrRXType);

	n = sizeof(csAddrRXTypeCombo)/sizeof(csAddrRXTypeCombo[0]);
	for (i=0; i<n;i++)
	{
		m_cbAddrRXType.AddString(csAddrRXTypeCombo[i]);
		m_cbAddrRXType.SetItemData(i, i);
	}
	m_cbAddrRXType.SetCurSel(nValue);

	// SQL
	if (pThisDoc)	nValue=pThisDoc->m_nSQL;
	else	nValue = DEF_PARAM_SQL;

	
	m_cbSQL.Clear();
	ClearComboBox(&m_cbSQL);

	n = sizeof(csSQL)/sizeof(csSQL[0]);
	for (i=0; i<n;i++)
	{
		m_cbSQL.AddString(csSQL[i]);
		m_cbSQL.SetItemData(i, i);
	}
	m_cbSQL.SetCurSel(nValue);

	// TX Mode	
	if (pThisDoc)	nValue=pThisDoc->m_nTXMode;
	else	nValue = DEF_PARAM_TXMODE;
	
	m_cbTXMode.Clear();
	ClearComboBox(&m_cbTXMode);

	n = sizeof(csTXMode)/sizeof(csTXMode[0]);
	for (i=0; i<n;i++)
	{		
		m_cbTXMode.AddString(csTXMode[i]);
		m_cbTXMode.SetItemData(i, i);
	}
	m_cbTXMode.SetCurSel(nValue);

	// Spec Symbol
	if (pThisDoc)	nValue=pThisDoc->m_nSpecSym;
	else	nValue = SPECSYM_DEFAULT;

	
	m_cbSpecSymbol.Clear();
	ClearComboBox(&m_cbSpecSymbol);

	n = sizeof(csNoneVisibleChar)/sizeof(csNoneVisibleChar[0]);
	for (i=0; i<n;i++)
	{		
		csTmp.Format(_T("%4d   %02X   %s"), i, i, csNoneVisibleChar[i].GetBuffer(0));
		m_cbSpecSymbol.AddString(csTmp);
		m_cbSpecSymbol.SetItemData(i, i);
	}

	for (i=n; i<256;i++)
	{		
		csTmp.Format(_T("%4d   %02X   %c"), i, i, i);
		m_cbSpecSymbol.AddString(csTmp);
		m_cbSpecSymbol.SetItemData(i, i);
	}

	m_cbSpecSymbol.SetCurSel(nValue);
	
	// Size of Buffer
	if (pThisDoc)	nValue=pThisDoc->m_nBufferSize;
	else	nValue = BUFFER_SIZE_DEFAULT;
	InitEditSpin(IDC_EDIT_BUFF_SIZE, IDC_SPIN_BUFF_SIZE, 						
								BUFFER_SIZE_MIN, BUFFER_SIZE_MAX, 1,
								nValue);	
	// Time out Last Byte
	if (pThisDoc)	nValue=pThisDoc->m_nTOLastByte;
	else	nValue = TO_LAST_BYTE_DEFAULT;
	InitEditSpin(IDC_EDIT_TIMEOUT_LASTBYTE, IDC_SPIN_TIMEOUT_LASTBYTE, 						
								TO_LAST_BYTE_MIN, TO_LAST_BYTE_MAX, TO_LAST_BYTE_STEP,
								nValue);

	// Remout Control
	if (pThisDoc)	nValue=pThisDoc->m_nRemoutControl;
	else	nValue = REMOUT_CONTROL_DEFAULT;
	m_btRemoutControl.SetCheck(nValue);

	// Local Echo
	if (pThisDoc)	nValue=pThisDoc->m_nLocalEcho;
	else	nValue = LOCAL_ECHO_DEFAULT;
	m_btLocalEcho.SetCheck(nValue);


	// Time out Pre Trans
	if (pThisDoc)	nValue=pThisDoc->m_nTOPreTrans;
	else	nValue = TO_PRETRANS_DEFAULT;
	InitEditSpin(IDC_EDIT_TO_PRETRANS, IDC_SPIN_TO_PRETRANS, 						
								TO_PRETRANS_MIN, TO_PRETRANS_MAX, TO_PRETRANS_STEP,
								nValue);

	// Time out Post Trans
	if (pThisDoc)	nValue=pThisDoc->m_nTOPostTrans;
	else	nValue = TO_POSTTRANS_DEFAULT;
	InitEditSpin(IDC_EDIT_TO_POSTTRANS, IDC_SPIN_TO_POSTTRANS, 						
								TO_POSTTRANS_MIN, TO_POSTTRANS_MAX, TO_POSTTRANS_STEP,
								nValue);

	// Modem Comm
	if (pThisDoc)	nValue=pThisDoc->m_nBaudRate;
	else	nValue = BAUD_RATE_DEFAULT;

	m_cbBaudRates.Clear();
	ClearComboBox(&m_cbBaudRates);

	n = sizeof(nBaudRates)/sizeof(nBaudRates[0]);
	for (i=0; i<n;i++)
	{
		csTmp.Format(_T("%d"), nBaudRates[i]);
		m_cbBaudRates.AddString(csTmp);
		m_cbBaudRates.SetItemData(i, i);
	}
	m_cbBaudRates.SetCurSel(nValue);

	if (pThisDoc)	m_nDataBits=pThisDoc->m_nDataBits;
	else	m_nDataBits = DATA_BITS_DEFAULT;

	CDataExchange *pDX = (CDataExchange*)new BYTE[sizeof(CDataExchange)];
	pDX->m_bSaveAndValidate = false;
	pDX->m_pDlgWnd = this;
	DDX_Radio(pDX, IDC_RADIO1, m_nDataBits); // DataBits

	if (pThisDoc)	m_nStopBits=pThisDoc->m_nStopBits;
	else	m_nStopBits = STOP_BITS_DEFAULT;
	pDX->m_bSaveAndValidate = false;
	pDX->m_pDlgWnd = this;
	DDX_Radio(pDX, IDC_RADIO5, m_nStopBits); // StopBits

	if (pThisDoc)	m_nParity=pThisDoc->m_nParity;
	else	m_nParity = PARITY_DEFAULT;

	pDX->m_bSaveAndValidate = false;
	pDX->m_pDlgWnd = this;
	DDX_Radio(pDX, IDC_RADIO7, m_nParity); // Parity

	delete pDX; 	
}


// CDMProgView diagnostics

#ifdef _DEBUG
void CDMProgView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDMProgView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDMProgDoc* CDMProgView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDMProgDoc)));
	return (CDMProgDoc*)m_pDocument;
}
#endif //_DEBUG


// CDMProgView message handlers

void CDMProgView::OnUpdateUIState(UINT nAction, UINT nUIElement)
{
	// This feature requires Windows 2000 or greater.
	// The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
	// TODO: Add your message handler code here

}

int CDMProgView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;	

	m_pSerialPort = NULL;	
	m_nCounter	= 0;
	m_nPowerOnCounter = 0;
	m_nState	= FSM_WAIT;
	m_nSubState	= 0;
	SetTimer(SERIAL_TIMER, SERIAL_TIMER_PERIOD, NULL);	

	return 0;
}

int CDMProgView::OnEnChangeEditInt(CEdit *pEdit, int nTrueValue, int nMax, int nMin, int nStep)
{
	// TODO:  Add your control notification handler code here
	CString						csStr1, csStr2;
	int								nValue;

	ASSERT_VALID(pEdit);

	pEdit->GetWindowText(csStr1);
	nValue = _ttoi(csStr1); 
	csStr2.Format(_T("%d"), nValue);
	if (csStr1.Compare(csStr2) == 0)
	{
		nTrueValue = nValue;
		if (nTrueValue >= nMax) nTrueValue = nMax;
		if (nTrueValue <= nMin) nTrueValue = nMin;

		nTrueValue = (nTrueValue/nStep)*nStep;

		if (nValue != nTrueValue)
		{		
			csStr1.Format(_T("%d"), nTrueValue);
			pEdit->SetWindowText(csStr1);		
		}
	}
	else
	{		
		csStr1.Format(_T("%d"), nTrueValue);
		pEdit->SetWindowText(csStr1);		
	}

	return nTrueValue;
}


void CDMProgView::OnEnChangeEditAddrSystem()
{
	// TODO:  Add your control notification handler code here
	CEdit							*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ADDR_SYSTEM);
	pThisDoc->m_nAddrSystem = OnEnChangeEditInt(pEdit, pThisDoc->m_nAddrSystem);
}
//-----------------------------------------------------------------------------

void CDMProgView::OnEnChangeEditAddrModem()
{
	// TODO:  Add your control notification handler code here
	CEdit							*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ADDR_MODEM);
	pThisDoc->m_nAddrModem = OnEnChangeEditInt(pEdit, pThisDoc->m_nAddrModem);
}

void CDMProgView::OnEnChangeEditAddrSender()
{
	// TODO:  Add your control notification handler code here
	CEdit							*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ADDR_SENDER);
	pThisDoc->m_nAddrSender = OnEnChangeEditInt(pEdit, pThisDoc->m_nAddrSender);
}

void CDMProgView::OnEnChangeEditAddrReceiver()
{
	// TODO:  Add your control notification handler code here
	CEdit							*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_ADDR_RECIEVER);
	pThisDoc->m_nAddrReceiver = OnEnChangeEditInt(pEdit, pThisDoc->m_nAddrReceiver);
}

void CDMProgView::OnBnClickedButton2()
{
	CString		csTmp;
	CButton		*pBtn;
	CCommDialog CommDlg;
	CWinApp* pApp = AfxGetApp();

	CommDlg.m_nRadioBtn = m_nPort-1;
	if (CommDlg.DoModal() == IDOK)
	{
		m_nPort = CommDlg.m_nRadioBtn+1;
		csTmp.Format(_T("COM%d"), m_nPort);
		pBtn = (CButton *)GetDlgItem(IDC_BUTTON_COMM);	
		if (pBtn) pBtn->SetWindowText(csTmp);
		
		pApp->WriteProfileInt(VERSION_NAME, PARAM_COMM_PORT, m_nPort);
	}
}

void CDMProgView::OnEnChangeEditBuffSize()
{
	// TODO:  Add your control notification handler code here
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_BUFF_SIZE);
	pThisDoc->m_nBufferSize = OnEnChangeEditInt(pEdit, pThisDoc->m_nBufferSize, BUFFER_SIZE_MAX, BUFFER_SIZE_MIN, 1);
}

void CDMProgView::OnEnChangeEditTimeoutLastbyte()
{
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TIMEOUT_LASTBYTE);
	pThisDoc->m_nTOLastByte = OnEnChangeEditInt(pEdit, pThisDoc->m_nTOLastByte, TO_LAST_BYTE_MAX, 0, 1);
}

void CDMProgView::OnEnKillfocusEditTimeoutLastbyte()
{
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TIMEOUT_LASTBYTE);
	pThisDoc->m_nTOLastByte = OnEnChangeEditInt(pEdit, pThisDoc->m_nTOLastByte, TO_LAST_BYTE_MAX, TO_LAST_BYTE_MIN, TO_LAST_BYTE_STEP);
}

void CDMProgView::OnEnChangeEditToPretrans()
{
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TO_PRETRANS);
	pThisDoc->m_nTOPreTrans = OnEnChangeEditInt(pEdit, pThisDoc->m_nTOPreTrans, TO_PRETRANS_MAX, TO_PRETRANS_MIN, 1);
}

void CDMProgView::OnEnKillfocusEditToPretrans()
{
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TO_PRETRANS);
	pThisDoc->m_nTOPreTrans = OnEnChangeEditInt(pEdit, pThisDoc->m_nTOPreTrans, TO_PRETRANS_MAX, TO_PRETRANS_MIN, TO_PRETRANS_STEP);
}

void CDMProgView::OnEnChangeEditToPosttrans()
{
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TO_POSTTRANS);
	pThisDoc->m_nTOPostTrans = OnEnChangeEditInt(pEdit, pThisDoc->m_nTOPostTrans, TO_POSTTRANS_MAX, TO_POSTTRANS_MIN, 1);
}

void CDMProgView::OnEnKillfocusEditToPosttrans()
{
	CEdit					*pEdit;
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TO_POSTTRANS);
	pThisDoc->m_nTOPostTrans = OnEnChangeEditInt(pEdit, pThisDoc->m_nTOPostTrans, TO_POSTTRANS_MAX, TO_POSTTRANS_MIN, TO_POSTTRANS_STEP);
}

void CDMProgView::LoadSettings(void)
{
	CWinApp* pApp = AfxGetApp();
  m_nPort = pApp->GetProfileInt(VERSION_NAME, PARAM_COMM_PORT, 1);
}


void CDMProgView::OnBnClickedButtonRead()
{
	CString				csTmp;
	CButton				*pBtn;

	m_dwLastStatusUpdate = GetTickCount();
	m_csNextStatusText	= _T("");
	m_dwTimeoutNext		= 3000;
	m_dwTimeout			= 3000;

  if (m_pSerialPort)
  {
    m_pSerialPort->StopMonitoring();
    delete m_pSerialPort;
    m_pSerialPort = NULL;
  }

	m_pSerialPort = new CSerialPort();
  if (!m_pSerialPort->InitPort(m_hWnd, m_nPort, 57600))
  {
		//csTmp.Format(_T("%s"), m_pSerialPort->m_LastError);
		csTmp = CA2W(m_pSerialPort->m_LastError);
		SetStatusText(csTmp);
    delete m_pSerialPort;
		m_pSerialPort = NULL;		
		return;
	}
  else
  {
		m_pSerialPort->StartMonitoring();
    m_pSerialPort->RestartMonitoring();
  }	

	SetToolBarOn(0);

	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_READ);	
	if (pBtn) pBtn->EnableWindow(0) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_WRITE);	
	if (pBtn) pBtn->EnableWindow(0) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_STOP);	
	if (pBtn) pBtn->EnableWindow(1) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_COMM);	
	if (pBtn) pBtn->EnableWindow(0) ;




	memset(m_bBuff, 0, sizeof(m_bBuff));
	memset(m_bPowerOnDetect, 0, sizeof(m_bPowerOnDetect));
	m_nCounter = 0;
	m_nPowerOnCounter = 0;
	m_nState = FSM_READ;
	m_bNeedWrite = FALSE;
	m_nSubState=0;
}

void CDMProgView::OnBnClickedButtonWrite()
{
	CString				csTmp;
	CButton				*pBtn;

	m_dwLastStatusUpdate = GetTickCount();
	m_csNextStatusText	= _T("");
	m_dwTimeoutNext		= 3000;
	m_dwTimeout			= 3000;

  if (m_pSerialPort)
  {
    m_pSerialPort->StopMonitoring();
    delete m_pSerialPort;
    m_pSerialPort = NULL;
  }

	m_pSerialPort = new CSerialPort();
  if (!m_pSerialPort->InitPort(m_hWnd, m_nPort, 57600))
  {
		//csTmp.Format(_T("%s"), m_pSerialPort->m_LastError);
		//SetStatusText(csTmp);
		csTmp = CA2W(m_pSerialPort->m_LastError);
		SetStatusText(csTmp);

    delete m_pSerialPort;
		m_pSerialPort = NULL;		
		return;
	}
  else
  {
		m_pSerialPort->StartMonitoring();
    m_pSerialPort->RestartMonitoring();
  }	

	SetToolBarOn(0);

	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_READ);	
	if (pBtn) pBtn->EnableWindow(0) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_WRITE);	
	if (pBtn) pBtn->EnableWindow(0) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_STOP);	
	if (pBtn) pBtn->EnableWindow(1) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_COMM);	
	if (pBtn) pBtn->EnableWindow(0) ;


	memset(m_bBuff, 0, sizeof(m_bBuff));
	memset(m_bPowerOnDetect, 0, sizeof(m_bPowerOnDetect));
	m_nCounter = 0;
	m_nPowerOnCounter = 0;
	m_nState = FSM_READ;
	m_nSubState=0;
	m_bNeedWrite = TRUE;
}

void CDMProgView::OnSerialTimer(void)
{
	CDMProgDoc				*pThisDoc;	
	SHORT	bState;
	BYTE	local_buff[BINARY_PACKET_MAX_SIZE];
	SHORT	local_size, size, n;
	BOOL	find_poweron;
	// Prcess packets
	find_poweron = (NULL != memmem(VERSION_NAME2, strlen(VERSION_NAME2), (char *)m_bPowerOnDetect, FINDBUFFER_SIZE));
	bState = try_receive_binary(m_bBuff, m_nCounter, local_buff, local_size);


	if (find_poweron)
	{
		memset(m_bPowerOnDetect, 0, sizeof(m_bPowerOnDetect));
		m_nPowerOnCounter=0;
		if (m_pSerialPort) m_pSerialPort->WriteToPort(SerialCommands_BIN_ON, sizeof(SerialCommands_BIN_ON));		
	}

	// Process state
	switch (m_nState)
	{
		case FSM_WAIT:
		{
			m_nProccessCounter=0;
			m_nCounter=0;
			m_nSubStateCounter=0;
			m_dwLastTickCount = GetTickCount();
			SetStatusProcess(_T(""));
		}
		break;
		case FSM_READ:
		{			
			n = sizeof(csProccess)/sizeof(csProccess[0]);
			m_nProccessCounter = (m_nProccessCounter+1) % n;
			SetStatusProcess(csProccess[m_nProccessCounter]);
			switch (m_nSubState)
			{
				case FSM_READ_WAIT_START:
				{
					if (m_nSubStateCounter < COUNT_READ_INFO_RETRY)
					{
						size = prepare_read_info(m_bTXBuff);
						if (m_pSerialPort) 
						m_pSerialPort->WriteToPort(m_bTXBuff, size);		
						m_dwLastTickCount = GetTickCount();
						m_nSubState = FSM_READ_READ_VERSION;

						SetStatusText(csStatuses[STATUS_READ_VERSION]);	
						if (find_poweron) m_nSubStateCounter = 0;
					}
					else
					{
						if ((GetTickCount() - m_dwLastTickCount) > m_dwTimeoutNext)
						{
							// exit
							SetStatusText(csStatuses[STATUS_ERR_NOANSWER]);
							OnBnClickedButtonStop();
						}
						else
						{
							if (find_poweron) m_nSubStateCounter = 0;
						}
					}
				}
				break;
				case FSM_READ_READ_VERSION:
				{
					if (bState == FSMS_BINARY_CMD_ACK)
					{
						if (local_size == (NAME_SIZE+2))
						{
							if ((memcmp(local_buff, MODEM_NAME, strlen(MODEM_NAME)) == 0) &&
								(local_buff[NAME_SIZE] == MODEM_VERSION_MAJOR) &&
								(local_buff[NAME_SIZE+1] == MODEM_VERSION_MINOR) )
							{
								// 
								m_nSubState = FSM_READ_READ_PROCESS;
								m_dwLastTickCount = GetTickCount();
								size = prepare_read_regs(m_bTXBuff);
								if (m_pSerialPort) 
								m_pSerialPort->WriteToPort(m_bTXBuff, size);
								SetStatusText(csStatuses[STATUS_READ_BASE]);
								m_nCounter = 0;
							}
							else
							{
								// ATHER VERSION								
								SetStatusText(csStatuses[STATUS_ERR_ATHER_VER]);
								OnBnClickedButtonStop();
							}
						}
					}
					else
					{
						if ((GetTickCount() - m_dwLastTickCount) >= TIMEOUT_READ_ACK)
						{
							m_nSubStateCounter++;
							m_nSubState = FSM_READ_WAIT_START;
							SetStatusText(csStatuses[STATUS_ERR_TIMEOUT]);
							if (m_nSubStateCounter >= COUNT_READ_INFO_RETRY)
							{
								// exit						
								SetStatusText(csStatuses[STATUS_ERR_NOANSWER]);
								m_csNextStatusText = csStatuses[STATUS_RESET_MODEM];
								m_dwTimeoutNext = 10000;
								m_dwLastTickCount = GetTickCount();
							}
						}
					}
				}
				break;
				case FSM_READ_READ_PROCESS:
				{
					
					if (bState == FSMS_BINARY_CMD_ACK)
					{
						if (local_size == NUM_OF_REGS)
						{															
							size = prepare_ack(m_bTXBuff);
							if (m_pSerialPort) 
							m_pSerialPort->WriteToPort(m_bTXBuff, size);
							m_nCounter = 0;
							SetStatusText(csStatuses[STATUS_READ_SUCC]);

							if (m_bNeedWrite)
							{
								// Read finish and start write 
								m_nCounter = 0;
								m_nPowerOnCounter = 0;
								m_nState = FSM_WRITE;
								m_nSubState=0;
								m_nSubStateCounter = 0;

								pThisDoc = GetDocument();
								ASSERT_VALID(pThisDoc);
								//memcpy(pThisDoc->m_bRegs, local_buff, size);
								pThisDoc->CheckDoc();
								pThisDoc->Doc2Buff();
								pThisDoc->StunParams(local_buff, local_size);


							}
							else
							{
								// Only read
								pThisDoc = GetDocument();
								ASSERT_VALID(pThisDoc);
								memcpy(pThisDoc->m_bRegs, local_buff, local_size);
								pThisDoc->Buff2Doc();
								pThisDoc->CheckDoc();

								OnBnClickedButtonStop();

								OnInitialUpdate();							
							}
						}
						else
						{
							// ATHER VERSION							
							SetStatusText(csStatuses[STATUS_ERR_ATHER_VER]);
							OnBnClickedButtonStop();
						}
					}
					else
					{
						if ((GetTickCount() - m_dwLastTickCount) >= TIMEOUT_READ_ACK)
						{							
							SetStatusText(csStatuses[STATUS_ERR_TIMEOUT]);
							OnBnClickedButtonStop();
						}
					}
				}
				break;
				default:
				{					
					m_nSubState = FSM_READ_WAIT_START;
					OnBnClickedButtonStop();
				}
			}
		}
		break;
		case FSM_WRITE:
		{
			n = sizeof(csProccess)/sizeof(csProccess[0]);
			m_nProccessCounter = (m_nProccessCounter+1) % n;
			SetStatusProcess(csProccess[m_nProccessCounter]);		

			switch (m_nSubState)
			{
				case FSM_WRITE_WAIT_START:
				{
					if (m_nSubStateCounter < COUNT_READ_INFO_RETRY)
					{
						pThisDoc = GetDocument();
						ASSERT_VALID(pThisDoc);						
						size = prepare_write_regs(m_bTXBuff, pThisDoc->m_bRegs);
						if (m_pSerialPort) 
						m_pSerialPort->WriteToPort(m_bTXBuff, size);		
						m_dwLastTickCount = GetTickCount();
						m_nSubState = FSM_WRITE_WRITE_PROCESS;

						SetStatusText(csStatuses[STATUS_WRITE_BASE]);							
					}
					else
					{
						// exit
						SetStatusText(csStatuses[STATUS_ERR_NOANSWER]);
						OnBnClickedButtonStop();
					}
				}
				break;
				case FSM_WRITE_WRITE_PROCESS:
				{				
					if (bState == FSMS_BINARY_CMD_ACK)
					{
						m_nCounter = 0;
						SetStatusText(csStatuses[STATUS_WRITE_SUCC]);

						OnBnClickedButtonStop();
					}
					else
					{
						if ((GetTickCount() - m_dwLastTickCount) >= TIMEOUT_READ_ACK)
						{							
							m_nSubStateCounter++;
							m_nSubState = FSM_WRITE_WAIT_START;
							SetStatusText(csStatuses[STATUS_ERR_TIMEOUT]);
						}
					}
				}
				break;
				default:
				{
					m_nSubState = FSM_READ_WAIT_START;
					OnBnClickedButtonStop();
				}			
			}

		}
		break;
		default:
		{			
			OnBnClickedButtonStop();
		}
	}

	if ((GetTickCount() - m_dwLastStatusUpdate) > m_dwTimeout)
	{
		m_dwTimeout = m_dwTimeoutNext;
		SetStatusText(m_csNextStatusText);	
	}
}
void CDMProgView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == SERIAL_TIMER)
		OnSerialTimer();

	CFormView::OnTimer(nIDEvent);
}
LRESULT CDMProgView::OnRXChar( WPARAM wParam, LPARAM lParam ) 
{
	BYTE ch = (BYTE)wParam;
	if (m_nPort == lParam)
	{
		if (m_nCounter < INBUFFER_SIZE)
		{
			m_bBuff[m_nCounter++] = ch;		
		}

		if (m_nPowerOnCounter < INBUFFER_SIZE)
		{
			m_bPowerOnDetect[m_nPowerOnCounter++] = ch;		
		}
		else
		{
			memmove(m_bPowerOnDetect, &m_bPowerOnDetect[m_nPowerOnCounter-INBUFFER_SIZE+1], INBUFFER_SIZE-1);
			m_bPowerOnDetect[INBUFFER_SIZE-1] = ch;
			m_nPowerOnCounter = INBUFFER_SIZE;
		}
	}

	return 0L;
}
//-----------------------------------------------------------------------------
char * memmem(char *m1, int size1, char *m2, int size2)
{
	//
	char *ptr=m2;
  int find;
	if ((m1!=NULL) && (m2!=NULL) && (size2>=size1))
	{
    ptr = (char *)memchr(ptr,*m1, size2);
    find = 0;
    while (!find && ptr)
    {
      // cmp
      if ((ptr-m2) <= (size2-size1))
      {
        find = (memcmp(ptr,m1,size1)==0);
        if (!find)
        {
          ptr++;
          ptr = (char *)memchr(ptr,*m1, size2);
        }
      }
      else ptr=NULL;
    }
	}
	else
	ptr = NULL;
  return ptr;
}
//-----------------------------------------------------------------------------
BYTE	easy_crc(BYTE		buf[], SHORT size)
{
	BYTE ecrc=0;
	SHORT ic;
	for (ic=0;ic<size;ic++)
	{
		ecrc -= buf[ic];
	}
	return ecrc;
}
//-----------------------------------------------------------------------------
SHORT try_receive_binary(BYTE	rx_modem_data[], SHORT &rx_len, BYTE	buf[], SHORT &size)
{
	SHORT 	i,find=0,res=FSMS_BINARY_CMD_NULL;
	SHORT	recv_size, recv_type, ecrc; 
		
	size=0;

	for (i=0;(i<rx_len) && !find;	i++)
	{
		find = (rx_modem_data[i] == BINARY_PACKET_HEADER);
	}
	
	if (!find)
	{
	  rx_len = 0;		
	}
	else
	{
		rx_len -= i-1;
		memmove(rx_modem_data, &rx_modem_data[i-1], rx_len);

		if (rx_len < BINARY_PACKET_MAX_SIZE)
		{
			if (rx_len >= BINARY_PACKET_MIN_SIZE)
			{
				recv_type = rx_modem_data[1];
				recv_size = rx_modem_data[2];

				// patch code
				if ((recv_type == FSMS_BINARY_CMD_ACK) && 
					(recv_size == BINARY_INFO_PACKET_SIZE))
				{
					recv_size = NAME_SIZE+2;	
				}

				if (rx_len >= (BINARY_PACKET_MIN_SIZE+recv_size))
				{
					ecrc = easy_crc(rx_modem_data, BINARY_PACKET_MIN_SIZE+recv_size);
					if (ecrc == 0)
					{
						res = recv_type;
						memcpy(buf, &rx_modem_data[3], recv_size);
						size = recv_size;
					}

					rx_len -= (BINARY_PACKET_MIN_SIZE+recv_size);
					memmove(rx_modem_data, &rx_modem_data[(BINARY_PACKET_MIN_SIZE+recv_size)], rx_len);										
				}
			}
		}
		else
		{
			rx_len -= 1;
			memmove(rx_modem_data, &rx_modem_data[1], rx_len);			
		}
	}
	return res;
}
//-----------------------------------------------------------------------------
SHORT prepare_write_regs(BYTE  buf[], BYTE regs[])
{
	SHORT res=0;
	buf[res] = BINARY_PACKET_HEADER;res++;
	buf[res] = FSMS_BINARY_CMD_WRITE_BASE;res++;	
	buf[res] = NUM_OF_REGS;res++;		
	memcpy(&buf[res], regs, NUM_OF_REGS);res+=NUM_OF_REGS;
	buf[res] = easy_crc(buf, res);res++;	
	return res;
}
//-----------------------------------------------------------------------------
SHORT prepare_read_regs(BYTE		buf[])
{
	SHORT res=0;
	buf[res] = BINARY_PACKET_HEADER;res++;
	buf[res] = FSMS_BINARY_CMD_READ_BASE;res++;	
	buf[res] = 0;res++;
	buf[res] = easy_crc(buf, res);res++;
	return res;
}
//-----------------------------------------------------------------------------
SHORT prepare_ack(BYTE		buf[])
{
	SHORT res=0;
	buf[res] = BINARY_PACKET_HEADER;res++;
	buf[res] = FSMS_BINARY_CMD_ACK;res++;
	buf[res] = 0;res++;
	buf[res] = easy_crc(buf, res);res++;
	return res;
}
//-----------------------------------------------------------------------------
SHORT prepare_nak(BYTE		buf[])
{
	SHORT res=0;
	buf[res] = BINARY_PACKET_HEADER;res++;
	buf[res] = FSMS_BINARY_CMD_NAK;res++;	
	buf[res] = 0;res++;		
	buf[res] = easy_crc(buf, res);res++;
	return res;
}
//-----------------------------------------------------------------------------
SHORT prepare_read_info(BYTE		buf[])
{
	SHORT res=0;
	buf[res] = BINARY_PACKET_HEADER;res++;
	buf[res] = FSMS_BINARY_CMD_INFO;res++;	
	buf[res] = 0;res++;		
	buf[res] = easy_crc(buf, res);res++;
	return res;
}
void CDMProgView::OnBnClickedButtonStop()
{
	CButton				*pBtn;

  if (m_pSerialPort)
  {
    m_pSerialPort->StopMonitoring();
    delete m_pSerialPort;
    m_pSerialPort = NULL;
  }

	SetToolBarOn(1);

	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_READ);	
	if (pBtn) pBtn->EnableWindow(1) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_WRITE);	
	if (pBtn) pBtn->EnableWindow(1) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_STOP);	
	if (pBtn) pBtn->EnableWindow(1) ;
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_COMM);	
	if (pBtn) pBtn->EnableWindow(1) ;


	memset(m_bBuff, 0, sizeof(m_bBuff));
	memset(m_bPowerOnDetect, 0, sizeof(m_bPowerOnDetect));
	m_nCounter = 0;
	m_nState = FSM_WAIT;
	m_dwLastStatusUpdate = GetTickCount();
	m_csNextStatusText	= _T("");
	m_dwTimeoutNext		= 3000;
	m_dwTimeout			= 3000;
}

void CDMProgView::OnCbnSelchangeComboAddrTx()
{
	CDMProgDoc				*pThisDoc;	
	INT							nCurSel;
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);
	nCurSel = m_cbAddrTXType.GetCurSel();
	pThisDoc->m_nAddrTXType = m_cbAddrTXType.GetItemData(nCurSel);	
}

void CDMProgView::OnCbnSelchangeComboAddrRx()
{
	CDMProgDoc				*pThisDoc;	
	INT							nCurSel;
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);
	nCurSel = m_cbAddrRXType.GetCurSel();
	pThisDoc->m_nAddrRXType = m_cbAddrRXType.GetItemData(nCurSel);	
}

void CDMProgView::OnCbnSelchangeComboSql()
{
	CDMProgDoc				*pThisDoc;	
	INT							nCurSel;
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);
	nCurSel = m_cbSQL.GetCurSel();
	pThisDoc->m_nSQL = m_cbSQL.GetItemData(nCurSel);	
}

void CDMProgView::OnCbnSelchangeComboTxmode()
{
	// TODO: Add your control notification handler code here
	CDMProgDoc				*pThisDoc;	
	INT							nCurSel;
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);
	nCurSel = m_cbTXMode.GetCurSel();
	pThisDoc->m_nTXMode = m_cbTXMode.GetItemData(nCurSel);	
}

void CDMProgView::OnCbnSelchangeComboSpecSym()
{
	// TODO: Add your control notification handler code here
	CDMProgDoc				*pThisDoc;	
	INT							nCurSel;
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);
	nCurSel = m_cbSpecSymbol.GetCurSel();
	pThisDoc->m_nSpecSym = m_cbSpecSymbol.GetItemData(nCurSel);	
}

void CDMProgView::OnBnClickedCheckLocalEcho()
{
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);	
	pThisDoc->m_nLocalEcho = m_btLocalEcho.GetCheck();
}

void CDMProgView::OnBnClickedCheckRemoveControl()
{
	CDMProgDoc				*pThisDoc;	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);	
	pThisDoc->m_nRemoutControl = m_btRemoutControl.GetCheck();
/*
	if (pThisDoc->m_nRemoutControl) MessageBox(_T("1"));
	else MessageBox(_T("0"));
*/
}

void CDMProgView::OnCbnSelchangeComboBaud()
{
	CDMProgDoc				*pThisDoc;	
	INT							nCurSel;
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);
	nCurSel = m_cbBaudRates.GetCurSel();
	pThisDoc->m_nBaudRate = m_cbBaudRates.GetItemData(nCurSel);	
}

void CDMProgView::OnBnClickedRadioModem()
{
	CDMProgDoc				*pThisDoc;	
	
	pThisDoc = GetDocument();
	ASSERT_VALID(pThisDoc);

	if (((CButton *)GetDlgItem(IDC_RADIO1))->GetCheck())	pThisDoc->m_nDataBits = 0;
	if (((CButton *)GetDlgItem(IDC_RADIO2))->GetCheck())	pThisDoc->m_nDataBits = 1;
	if (((CButton *)GetDlgItem(IDC_RADIO3))->GetCheck())	pThisDoc->m_nDataBits = 2;
	if (((CButton *)GetDlgItem(IDC_RADIO4))->GetCheck())	pThisDoc->m_nDataBits = 3;

	if (((CButton *)GetDlgItem(IDC_RADIO5))->GetCheck())	pThisDoc->m_nStopBits = 0;
	if (((CButton *)GetDlgItem(IDC_RADIO6))->GetCheck())	pThisDoc->m_nStopBits = 1;

	if (((CButton *)GetDlgItem(IDC_RADIO7))->GetCheck())	pThisDoc->m_nParity = 0;
	if (((CButton *)GetDlgItem(IDC_RADIO8))->GetCheck())	pThisDoc->m_nParity = 1;
	if (((CButton *)GetDlgItem(IDC_RADIO9))->GetCheck())	pThisDoc->m_nParity = 2;

}
void CDMProgView::SetStatusText(CString csParam)
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, AfxGetApp()->m_pMainWnd);
	if (pFrame)
	{
		m_dwLastStatusUpdate = GetTickCount();
		pFrame->SetStatusText(csParam);
	}
}

void CDMProgView::SetStatusProcess(CString csParam)
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, AfxGetApp()->m_pMainWnd);
	if (pFrame)
	{
		pFrame->SetStatusProcess(csParam);
	}
}

void CDMProgView::SetToolBarOn(BOOL bOn)
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, AfxGetApp()->m_pMainWnd);
	if (pFrame)
	{
		pFrame->SetEnableToolbar(bOn);
	}
}
void CDMProgView::OnHelpUpdate()
{
	CUpdateFirmWare	UpdateFrm;

	UpdateFrm.m_nPort = m_nPort;
	UpdateFrm.m_csFirmWarePath = m_csFirmWarePath;		
	UpdateFrm.DoModal();
	m_csFirmWarePath = UpdateFrm.m_csFirmWarePath;

	if (UpdateFrm.m_dwResult == UPDATE_FIRMWARE_ERROR_ID__SUCCESS)
	{
		MessageBox(UPDATE_FIRMWARE_ERROR__SUCCESS);
	}
}


