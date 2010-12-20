
// DMProgDoc.h : interface of the CDMProgDoc class
//


#pragma once

#include "xmlarchive.h"
#include "DMProgParam.h"

class CDMProgDoc : public CDocument
{
protected: // create from serialization only
	CDMProgDoc();
	DECLARE_XMLSERIAL(CDMProgDoc)

// Attributes
public:
	CString	m_csModemName;
	int			m_nMajor;
	int			m_nMinor;
	int			m_nValidVersion;

	int			m_nAddrSystem;
	int			m_nAddrModem;
	int			m_nAddrSender;
	int			m_nAddrReceiver;

	int			m_nAddrTXType;
	int			m_nAddrRXType;

	int			m_nSQL;
	int			m_nTXMode;

	int			m_nBaudRate;
	int			m_nDataBits;
	int			m_nStopBits;
	int			m_nParity;
	int			m_nSpecSym;
	int			m_nBufferSize;
	int			m_nTOLastByte;
	int			m_nRemoutControl;
	int			m_nLocalEcho;
	int			m_nTOPreTrans;
	int			m_nTOPostTrans;

	BYTE		m_bRegs[NUM_OF_REGS];

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);	
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();


// Implementation
public:
	virtual ~CDMProgDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void Doc2Buff(void);
	void Buff2Doc(void);
	void CheckDoc(void);
	void StunParams(BYTE bRegs[], SHORT size);
protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};


