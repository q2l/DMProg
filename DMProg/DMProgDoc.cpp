
// DMProgDoc.cpp : implementation of the CDMProgDoc class
//

#include "stdafx.h"
#include "DMProg.h"

#include "DMProgDoc.h"
#include "DMProgParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDMProgDoc

IMPLEMENT_XMLSERIAL(CDMProgDoc, CDocument)

BEGIN_MESSAGE_MAP(CDMProgDoc, CDocument)
END_MESSAGE_MAP()


// CDMProgDoc construction/destruction

CDMProgDoc::CDMProgDoc()
{
	// TODO: add one-time construction code here

}

CDMProgDoc::~CDMProgDoc()
{
}

BOOL CDMProgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_csModemName = MODEM_NAME;
	m_nMajor = MODEM_VERSION_MAJOR;
	m_nMinor = MODEM_VERSION_MINOR;

	m_nAddrSystem		= DEF_PARAM_ADDR_SYSTEM;
	m_nAddrModem		= DEF_PARAM_ADDR_MODEM;
	m_nAddrSender		=	DEF_PARAM_ADDR_SENDER;
	m_nAddrReceiver	=	DEF_PARAM_ADDR_RECEIVER;

	m_nAddrTXType		= DEF_PARAM_ADDR_TX;
	m_nAddrRXType		= DEF_PARAM_ADDR_RX;

	m_nSQL				= DEF_PARAM_SQL;
	m_nTXMode			= DEF_PARAM_TXMODE;

	m_nBaudRate			= BAUD_RATE_DEFAULT;
	m_nDataBits			= DATA_BITS_DEFAULT;
	m_nStopBits			= STOP_BITS_DEFAULT;
	m_nParity			= PARITY_DEFAULT;

	m_nSpecSym			= SPECSYM_DEFAULT;

	m_nBufferSize		= BUFFER_SIZE_DEFAULT;
	m_nTOLastByte		= TO_LAST_BYTE_DEFAULT;
	m_nRemoutControl	= REMOUT_CONTROL_DEFAULT;
	m_nLocalEcho		= LOCAL_ECHO_DEFAULT;
	m_nTOPreTrans		= TO_PRETRANS_DEFAULT;
	m_nTOPostTrans		= TO_POSTTRANS_DEFAULT;

	return TRUE;
}




// CDMProgDoc serialization

void CDMProgDoc::Serialize(CArchive& ar)
{
	CString ModemName;
	int			Major, Minor;

	if (ar.IsStoring())
	{
		
		// Saveing
		ModemName = m_csModemName;
		Major			= m_nMajor;
		Minor			= m_nMinor;

		XMLCLASSNODE;
		XMLDATA(ModemName);
		XMLDATA(Major);
		XMLDATA(Minor);

		
		XMLDATA(m_nAddrSystem);
		XMLDATA(m_nAddrModem);
		XMLDATA(m_nAddrSender);
		XMLDATA(m_nAddrReceiver);

		XMLDATA(m_nAddrTXType);
		XMLDATA(m_nAddrRXType);

		XMLDATA(m_nSQL);
		XMLDATA(m_nTXMode);

		XMLDATA(m_nBaudRate);
		XMLDATA(m_nDataBits);
		XMLDATA(m_nStopBits);
		XMLDATA(m_nParity);
		XMLDATA(m_nSpecSym);
		XMLDATA(m_nBufferSize);
		XMLDATA(m_nTOLastByte);
		XMLDATA(m_nRemoutControl);
		XMLDATA(m_nLocalEcho);
		XMLDATA(m_nTOPreTrans);
		XMLDATA(m_nTOPostTrans);

		XMLENDNODE;
	}
	else
	{
		// Loading
		XMLCLASSNODE;
		XMLDATA(ModemName);
		XMLDATA(Major);
		XMLDATA(Minor);

		if ((m_csModemName.Compare(ModemName) == 0) &&
				(Major == m_nMajor) &&
				(Minor == m_nMinor)
			)
		{			
			XMLDATA(m_nAddrSystem);
			XMLDATA(m_nAddrModem);
			XMLDATA(m_nAddrSender);
			XMLDATA(m_nAddrReceiver);

			XMLDATA(m_nAddrTXType);
			XMLDATA(m_nAddrRXType);

			XMLDATA(m_nSQL);
			XMLDATA(m_nTXMode);

			XMLDATA(m_nBaudRate);
			XMLDATA(m_nDataBits);
			XMLDATA(m_nStopBits);
			XMLDATA(m_nParity);
			XMLDATA(m_nSpecSym);
			XMLDATA(m_nBufferSize);
			XMLDATA(m_nTOLastByte);
			XMLDATA(m_nRemoutControl);
			XMLDATA(m_nLocalEcho);
			XMLDATA(m_nTOPreTrans);
			XMLDATA(m_nTOPostTrans);
			m_nValidVersion = 1;	
		}
		else
		{
			m_nValidVersion = 0;	
		}

		XMLENDNODE;
	}
}


BOOL CDMProgDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	TRY
	{
		CWaitCursor wait;
		CXMLArchive xmlArchive(lpszPathName, CArchive::store, NULL, this);
		Serialize(xmlArchive);     // save me
	}
	CATCH_ALL(e)
	{
		TRY
		{
			ReportSaveLoadException(lpszPathName, e,
				TRUE, AFX_IDP_FAILED_TO_SAVE_DOC);
		}
		END_TRY
		delete e;
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;        // success
}

BOOL CDMProgDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_nValidVersion=0;

	TRY
	{
		CWaitCursor wait;
		CXMLArchive xmlArchive(lpszPathName, CArchive::load, NULL, this);
		Serialize(xmlArchive);     // load me
	}
	CATCH_ALL(e)
	{

		ReportSaveLoadException(lpszPathName, e,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);

		delete e;
		return FALSE;
	}
	END_CATCH_ALL

	if (!m_nValidVersion)
	{
		ReportSaveLoadException(lpszPathName, NULL,
			FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);	
	}
	
	return m_nValidVersion;
}

void CDMProgDoc::OnCloseDocument()
{
	//	

	CDocument::OnCloseDocument();
}


// CDMProgDoc diagnostics

#ifdef _DEBUG
void CDMProgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDMProgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CDMProgDoc::Doc2Buff(void)
{
	BYTE bibi;
	memset(m_bRegs, 0, sizeof(m_bRegs));

	m_bRegs[11]	=	(m_nSQL != 0);
	m_bRegs[13] = m_nAddrRXType & 0x07;
	m_bRegs[14] = m_nTXMode & 0x03;
	bibi = ((m_nParity==0) ? 0 : m_nParity+1);
	m_bRegs[15] = ((m_nDataBits & 0x03)<<1) | ((m_nStopBits & 0x01)<<3) | ((bibi & 0x03) << 4);

	m_bRegs[16] = m_nBaudRate;
	m_bRegs[18] = 0x00;

	m_bRegs[24] = m_nSpecSym;
	m_bRegs[25] = m_nAddrModem;
	m_bRegs[26] = m_nAddrReceiver;
	m_bRegs[27] = m_nAddrSender;
	m_bRegs[28] = m_nTOPostTrans/TO_POSTTRANS_STEP;
	m_bRegs[29] = m_nTOPreTrans/TO_PRETRANS_STEP;
	m_bRegs[30] = ((m_nLocalEcho != 0) ? 0xF0 : 0x00);
	m_bRegs[31] = m_nTOLastByte/TO_LAST_BYTE_STEP;
	
	m_bRegs[35] = 0x00;

	m_bRegs[39] = m_nRemoutControl != 0;
	m_bRegs[40] = m_nAddrSystem;

	m_bRegs[42] = (m_nBufferSize)/BUFFER_SIZE_STEP;

	m_bRegs[53] = m_nAddrTXType;
}

void CDMProgDoc::Buff2Doc(void)
{
	m_nSQL 					=	(m_bRegs[11] != 0);
	m_nAddrRXType		= m_bRegs[13] & 0x07;
	m_nTXMode				=	m_bRegs[14] & 0x03;
	m_nDataBits			= (m_bRegs[15]>>1)&0x03;
	m_nStopBits			= (m_bRegs[15]>>3)&0x01;
	m_nParity				= (m_bRegs[15]>>4)&0x03;
	if (m_nParity > 0) m_nParity--;

	m_nBaudRate			= m_bRegs[16];

	m_nSpecSym			= m_bRegs[24];
	m_nAddrModem		= m_bRegs[25];
	m_nAddrReceiver	= m_bRegs[26];
	m_nAddrSender		= m_bRegs[27];
	m_nTOPostTrans	= m_bRegs[28]*TO_POSTTRANS_STEP;
	m_nTOPreTrans		= m_bRegs[29]*TO_PRETRANS_STEP;
	m_nLocalEcho		= m_bRegs[30] != 0;
	m_nTOLastByte		=	m_bRegs[31]*TO_LAST_BYTE_STEP;
	
	m_nRemoutControl= (m_bRegs[39] != 0);
	m_nAddrSystem		= m_bRegs[40];

	m_nBufferSize		= m_bRegs[42]*BUFFER_SIZE_STEP;

	m_nAddrTXType		= m_bRegs[53];
}

void CDMProgDoc::CheckDoc(void)
{
	if (m_nSQL > SQL_MAX) m_nSQL = DEF_PARAM_SQL;
	if (m_nSQL < SQL_MIN) m_nSQL = DEF_PARAM_SQL;
	
	if (m_nAddrRXType > ADDR_RX_MAX) m_nAddrRXType = DEF_PARAM_ADDR_RX;
	if (m_nAddrRXType < ADDR_RX_MIN) m_nAddrRXType = DEF_PARAM_ADDR_RX;

	if (m_nAddrTXType > ADDR_TX_MAX) m_nAddrTXType = DEF_PARAM_ADDR_TX;
	if (m_nAddrTXType < ADDR_TX_MIN) m_nAddrTXType = DEF_PARAM_ADDR_TX;

	if (m_nAddrModem > BYTE_RANGE_MAX) m_nAddrModem = DEF_PARAM_ADDR_MODEM;
	if (m_nAddrModem < BYTE_RANGE_MIN) m_nAddrModem = DEF_PARAM_ADDR_MODEM;

	if (m_nAddrReceiver > BYTE_RANGE_MAX) m_nAddrReceiver = DEF_PARAM_ADDR_RECEIVER;
	if (m_nAddrReceiver < BYTE_RANGE_MIN) m_nAddrReceiver = DEF_PARAM_ADDR_RECEIVER;

	if (m_nAddrSender > BYTE_RANGE_MAX) m_nAddrSender = DEF_PARAM_ADDR_SENDER;
	if (m_nAddrSender < BYTE_RANGE_MIN) m_nAddrSender = DEF_PARAM_ADDR_SENDER;

	if (m_nAddrSystem > BYTE_RANGE_MAX) m_nAddrSystem = DEF_PARAM_ADDR_SYSTEM;
	if (m_nAddrSystem < BYTE_RANGE_MIN) m_nAddrSystem = DEF_PARAM_ADDR_SYSTEM;

	if (m_nTXMode > TXMODE_MAX) m_nTXMode = DEF_PARAM_TXMODE;
	if (m_nTXMode < TXMODE_MIN) m_nTXMode = DEF_PARAM_TXMODE;

	if (m_nBaudRate > BAUD_RATE_MAX) m_nBaudRate = BAUD_RATE_DEFAULT;
	if (m_nBaudRate < BAUD_RATE_MIN) m_nBaudRate = BAUD_RATE_DEFAULT;

	if (m_nDataBits > DATA_BITS_MAX) m_nDataBits = DATA_BITS_DEFAULT;
	if (m_nDataBits < DATA_BITS_MIN) m_nDataBits = DATA_BITS_DEFAULT;

	if (m_nStopBits > STOP_BITS_MAX) m_nStopBits = STOP_BITS_DEFAULT;
	if (m_nStopBits < STOP_BITS_MIN) m_nStopBits = DATA_BITS_DEFAULT;

	if (m_nParity > PARITY_MAX) m_nParity = PARITY_DEFAULT;
	if (m_nParity < PARITY_MIN) m_nParity = PARITY_DEFAULT;

	if (m_nSpecSym > SPECSYM_MAX) m_nSpecSym = SPECSYM_DEFAULT;
	if (m_nSpecSym < SPECSYM_MIN) m_nSpecSym = SPECSYM_DEFAULT;

	if (m_nBufferSize > BUFFER_SIZE_MAX) m_nBufferSize = BUFFER_SIZE_DEFAULT;
	if (m_nBufferSize < BUFFER_SIZE_MIN) m_nBufferSize = BUFFER_SIZE_DEFAULT;

	if (m_nTOLastByte > TO_LAST_BYTE_MAX) m_nTOLastByte = TO_LAST_BYTE_DEFAULT;
	if (m_nTOLastByte < TO_LAST_BYTE_MIN) m_nTOLastByte = TO_LAST_BYTE_DEFAULT;


	if (m_nRemoutControl > REMOUT_CONTROL_MAX) m_nRemoutControl = REMOUT_CONTROL_DEFAULT;
	if (m_nRemoutControl < REMOUT_CONTROL_MIN) m_nRemoutControl = REMOUT_CONTROL_DEFAULT;

	if (m_nLocalEcho > LOCAL_ECHO_MAX) m_nLocalEcho = LOCAL_ECHO_DEFAULT;
	if (m_nLocalEcho < LOCAL_ECHO_MIN) m_nLocalEcho = LOCAL_ECHO_DEFAULT;

	if (m_nTOPostTrans > TO_POSTTRANS_MAX) m_nTOPostTrans = TO_POSTTRANS_DEFAULT;
	if (m_nTOPostTrans < TO_POSTTRANS_MIN) m_nTOPostTrans = TO_POSTTRANS_DEFAULT;

	if (m_nTOPreTrans > TO_PRETRANS_MAX) m_nTOPreTrans = TO_PRETRANS_DEFAULT;
	if (m_nTOPreTrans < TO_PRETRANS_MIN) m_nTOPreTrans = TO_PRETRANS_DEFAULT;
}

void CDMProgDoc::StunParams(BYTE bRegs[], SHORT size)
{
	if (size >= NUM_OF_REGS)
	{
		m_bRegs[98]=bRegs[98];
		m_bRegs[99]=bRegs[99];
	}
}
// CDMProgDoc commands
