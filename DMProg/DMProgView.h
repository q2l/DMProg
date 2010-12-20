
// DMProgView.h : interface of the CDMProgView class
//


#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "DMProgParam.h"
#include "SerialPort.h"


#define SERIAL_COMMAND_BINARY_ON_SIZE	1


#define SERIAL_TIMER					1
#define SERIAL_TIMER_PERIOD		50
#define INBUFFER_SIZE					1024
#define FINDBUFFER_SIZE				(INBUFFER_SIZE+32)

SHORT try_receive_binary(BYTE	rx_modem_data[], SHORT &rx_len, BYTE	buf[], SHORT &size);
char * memmem(char *m1, int size1, char *m2, int size2);

SHORT prepare_ack(BYTE		buf[]);
SHORT prepare_nak(BYTE		buf[]);
SHORT prepare_read_regs(BYTE		buf[]);
SHORT prepare_write_regs(BYTE  buf[], BYTE regs[]);
SHORT prepare_read_info(BYTE		buf[]);

class CDMProgView : public CFormView
{
protected: // create from serialization only
	CDMProgView();
	DECLARE_DYNCREATE(CDMProgView)

public:
	enum{ IDD = IDD_DMPROG_FORM };

// Attributes
public:
	CDMProgDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CDMProgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
		void InitEditSpin(int nIDEdit, int nIDSpin, int nMin, int nMax, int nStep, int nValue);
		void LoadSettings(void);	

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg int OnEnChangeEditInt(CEdit *pEdit, int nTrueValue, int nMax=255, int nMin=0, int nStep=1);
	afx_msg void OnEnChangeEditAddrSystem();
	afx_msg void OnEnChangeEditAddrModem();
	afx_msg void OnEnChangeEditAddrSender();
	afx_msg void OnEnChangeEditAddrReceiver();
	afx_msg void OnCbnSelchangeComboAddrTx();

	CString			m_csFirmWarePath;

	CSerialPort			*m_pSerialPort;
	int					m_nPort;
	BYTE				m_bBuff[INBUFFER_SIZE];	
	BYTE				m_bTXBuff[INBUFFER_SIZE];	
	SHORT				m_nCounter;
	SHORT				m_nPowerOnCounter;
	BYTE				m_bPowerOnDetect[FINDBUFFER_SIZE];
	INT					m_nState;
	INT					m_nSubState;
	INT					m_nSubStateCounter;
	DWORD				m_dwLastTickCount;


	INT					m_nProccessCounter;
	DWORD				m_dwLastStatusUpdate;
	CString				m_csNextStatusText;
	DWORD				m_dwTimeoutNext;
	DWORD				m_dwTimeout;

	BOOL				m_bNeedWrite;

	CComboBox m_cbAddrTXType;
	CComboBox m_cbAddrRXType;
	CComboBox m_cbSQL;
	CComboBox m_cbTXMode;
	afx_msg void OnBnClickedButton2();
	int m_nDataBits;	
	int m_nStopBits;
	int m_nParity;
	CComboBox m_cbBaudRates;
	CComboBox m_cbSpecSymbol;
	CButton m_btRemoutControl;
	CButton m_btLocalEcho;

	void SetStatusText(CString csParam);
	void SetStatusProcess(CString csParam);
	void SetToolBarOn(BOOL bOn);

	afx_msg void OnEnChangeEditBuffSize();
	afx_msg void OnEnChangeEditTimeoutLastbyte();
	afx_msg void OnEnKillfocusEditTimeoutLastbyte();
	afx_msg void OnEnChangeEditToPretrans();
	afx_msg void OnEnKillfocusEditToPretrans();
	afx_msg void OnEnChangeEditToPosttrans();
	afx_msg void OnEnKillfocusEditToPosttrans();
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonWrite();
	void OnSerialTimer(void);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnRXChar(WPARAM wParam, LPARAM lParam );
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnCbnSelchangeComboAddrRx();
	afx_msg void OnCbnSelchangeComboSql();
	afx_msg void OnCbnSelchangeComboTxmode();
	afx_msg void OnCbnSelchangeComboSpecSym();
	afx_msg void OnBnClickedCheckLocalEcho();
	afx_msg void OnBnClickedCheckRemoveControl();
	afx_msg void OnCbnSelchangeComboBaud();
	afx_msg void OnBnClickedRadioModem();
	afx_msg void OnHelpUpdate();
};

#ifndef _DEBUG  // debug version in DMProgView.cpp
inline CDMProgDoc* CDMProgView::GetDocument() const
   { return reinterpret_cast<CDMProgDoc*>(m_pDocument); }
#endif

