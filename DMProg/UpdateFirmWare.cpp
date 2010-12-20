// UpdateFirmWare.cpp : implementation file
//

#include "stdafx.h"
#include "DMProg.h"
#include "UpdateFirmWare.h"


// CUpdateFirmWare dialog

UINT __cdecl ThreadProccessSerial( LPVOID pParam );

IMPLEMENT_DYNAMIC(CUpdateFirmWare, CDialog)

CUpdateFirmWare::CUpdateFirmWare(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateFirmWare::IDD, pParent)
	,m_pSerialPort(NULL)
	,m_nCounter(0)
	,m_dwResult(UPDATE_FIRMWARE_ERROR_ID__CANCEL)
{

}

CUpdateFirmWare::~CUpdateFirmWare()
{
}

void CUpdateFirmWare::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ProgressGuage);
}

BOOL CUpdateFirmWare::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_EDIT_PATH, m_csFirmWarePath);

	ZeroMemory(&m_ofnParam, sizeof(m_ofnParam));
	m_ofnParam.lStructSize			= sizeof(m_ofnParam);
	m_ofnParam.hwndOwner				= m_hWnd;
	m_ofnParam.lpstrFilter			= _T("LDR file(*.ldr)\0*.ldr\0\0");
	m_ofnParam.nFilterIndex			= 0;
	m_ofnParam.lpstrFile				= m_csFirmWarePath.GetBuffer(0);
	m_ofnParam.nMaxFile					= 4096;
	m_ofnParam.Flags						= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	m_dwTimeOut									= 10;

	m_pCS = new CRITICAL_SECTION;
	InitializeCriticalSection(m_pCS);

	m_pThreadProc = AfxBeginThread(ThreadProccessSerial, this, 0, 0, CREATE_SUSPENDED);
	if (m_pThreadProc)
	{
		m_pThreadProc->ResumeThread();
	}

	return TRUE;
}

BOOL CUpdateFirmWare::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
  if(pMsg->message==WM_KEYDOWN)
  {
		if(pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE)
    pMsg->wParam=NULL ;
   }
   return CDialog::PreTranslateMessage(pMsg);
}


BEGIN_MESSAGE_MAP(CUpdateFirmWare, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_DIALOG, &CUpdateFirmWare::OnBnClickedButtonOpenDialog)
	ON_BN_CLICKED(ID_BUTTON_UPDATE, &CUpdateFirmWare::OnBnClickedButtonUpdate)
	ON_MESSAGE(WM_COMM_RXCHAR, OnRXChar)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDCANCEL, &CUpdateFirmWare::OnBnClickedCancel)
END_MESSAGE_MAP()


// CUpdateFirmWare message handlers

void CUpdateFirmWare::OnBnClickedButtonOpenDialog()
{
	TCHAR szFileName[4096];
	memset(szFileName, 0, sizeof(szFileName));
	strcpy_s(szFileName, 4095, m_csFirmWarePath.GetBuffer(0));
	m_ofnParam.lpstrFile = szFileName;
	if (GetOpenFileName(&m_ofnParam)){
		
		SetDlgItemText(IDC_EDIT_PATH, m_ofnParam.lpstrFile);
		GetDlgItemText(IDC_EDIT_PATH, m_csFirmWarePath);
	}
}


bool FileExist( LPCTSTR filename )
{
	// Data structure for FindFirstFile
	WIN32_FIND_DATA findData;
	// Clear find structure
	ZeroMemory(&findData, sizeof(findData));
	// Search the file
	HANDLE hFind = FindFirstFile( filename, &findData );
	if ( hFind == INVALID_HANDLE_VALUE )
	{
		// File not found
		return false;
	}
	// Release find handle
	FindClose( hFind );
	hFind = NULL;
	// The file exists
	return true;
}


void CUpdateFirmWare::OnBnClickedButtonUpdate()
{
	CButton *pBtn;
	CString csTmp;
	GetDlgItemText(IDC_EDIT_PATH, m_csFirmWarePath);

	if (!FileExist(m_csFirmWarePath))
	{
		SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__FILE_NOT_FOUND);
		return;
	}

  if (m_pSerialPort)
  {
    m_pSerialPort->StopMonitoring();
    delete m_pSerialPort;
    m_pSerialPort = NULL;
  }

	m_pSerialPort = new CSerialPort();
  if (!m_pSerialPort->InitPort(m_hWnd, m_nPort, 57600))
  {
		SetDlgItemText(IDC_STATIC_PROCESS, m_pSerialPort->m_LastError);
    delete m_pSerialPort;
		m_pSerialPort = NULL;		
		return;
	}
  else
  {
		m_pSerialPort->StartMonitoring();
    m_pSerialPort->RestartMonitoring();
  }	

	m_nState = FSM_UPDATE_INIT;
	SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__POWERON);
	pBtn = (CButton *)GetDlgItem(IDC_BUTTON_OPEN_DIALOG);
	if (pBtn) pBtn->EnableWindow(0);
	pBtn = (CButton *)GetDlgItem(ID_BUTTON_UPDATE);
	if (pBtn) pBtn->EnableWindow(0);
}

//-----------------------------------------------------------------------------


UINT __cdecl ThreadProccessSerial( LPVOID pParam )
{
	HANDLE						hFakeEvent;
	CUpdateFirmWare		*pDlg = (CUpdateFirmWare *)pParam;
	CButton						*pBtn;
	BYTE							szTXBuffer[BUFFER_SIZE];
	BYTE							szRXBuffer[BUFFER_SIZE];
	BYTE							szMustRXBuffer[BUFFER_SIZE];
	int								nReadSize=0, nWriteSize;
	int								nSize=0, nMustRxSize;
	CFile							*pLDRFile=NULL;
	BYTE							szFileBuffer[FILE_BUFFER_SIZE];
	int								nFileBufferSize, nLineIndex;
	ULONGLONG					nFileSize, nCurrSize;
	int								nScanRes;
	int               nbytes_rcv, nkb_rcv;
	int               npage_trm, nbytes_trm, nkb_trm;
	unsigned long     dwstdcrc;

	CString						csProcessMsg;
	double						dPersent;
	int								nPersent;

	
	
	if (pDlg == NULL) return 0;

	hFakeEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	while (1)
	{		
		WaitForSingleObject(hFakeEvent, pDlg->m_dwTimeOut);

		switch (pDlg->m_nState)
		{
			case FSM_UPDATE_WAIT:
			{
				pDlg->FlushRxBuffer();
				pDlg->m_dwLastTickCount=GetTickCount();

				pBtn = (CButton *)pDlg->GetDlgItem(IDC_BUTTON_OPEN_DIALOG);
				if (pBtn) pBtn->EnableWindow(1);
				pBtn = (CButton *)pDlg->GetDlgItem(ID_BUTTON_UPDATE);
				if (pBtn) pBtn->EnableWindow(1);

				if (pDlg->m_pSerialPort)
				{
					pDlg->m_pSerialPort->StopMonitoring();
					delete pDlg->m_pSerialPort;
					pDlg->m_pSerialPort = NULL;
				}
			}
			break;
			case FSM_UPDATE_INIT:
			{
				nReadSize = pDlg->ReadRxBuffer(&szRXBuffer[nSize], BUFFER_SIZE);
				nSize += nReadSize;
				if (memmem(ERROR_COMMAND, strlen(ERROR_COMMAND), (char *)szRXBuffer, nSize) != NULL)
				{
					pDlg->m_nState = FSM_UPDATE_START_DL;
					pDlg->m_nSubState = 0;
					pDlg->m_dwLastTickCount = GetTickCount();

					nSize = 0;
					nWriteSize = strlen(ENTER_PASS);
					memcpy(szTXBuffer, ENTER_PASS, nWriteSize);
					if (pDlg->m_pSerialPort)
					pDlg->m_pSerialPort->WriteToPort(szTXBuffer, nWriteSize);
				}
				else
				{
					memset(szTXBuffer, PROG_INIT_CH, PROG_INIT_PACKET_SIZE);
					pDlg->m_pSerialPort->WriteToPort(szTXBuffer, PROG_INIT_PACKET_SIZE);

					if (nSize > BUFFER_SIZE2)
					{
						memmove(szRXBuffer, &szRXBuffer[BUFFER_SIZE4], nSize-BUFFER_SIZE4);
						nSize-=BUFFER_SIZE4;
					}

					if ((GetTickCount() - pDlg->m_dwLastTickCount) > WAIT_TO_START_TO)
					{
						// Exit
						pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE);
						pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE;
						pDlg->m_nState = FSM_UPDATE_WAIT;
					}
				}
			}
			break;
			case FSM_UPDATE_START_DL:
			{
				nReadSize = pDlg->ReadRxBuffer(&szRXBuffer[nSize], BUFFER_SIZE);
				nSize += nReadSize;
				if (memmem(ERROR_COMMAND, strlen(ERROR_COMMAND), (char *)szRXBuffer, nSize) != NULL)
				{
					pDlg->m_nState = FSM_UPDATE_CONF_DL;
					pDlg->m_nSubState = 0;
					pDlg->m_dwLastTickCount = GetTickCount();

					nSize = 0;
					nWriteSize = strlen(DL_PASS);
					memcpy(szTXBuffer, DL_PASS, nWriteSize);
					pDlg->m_pSerialPort->WriteToPort(szTXBuffer, nWriteSize);
				}
				else
				{
					nWriteSize = strlen(ENTER_PASS);
					memcpy(szTXBuffer, ENTER_PASS, nWriteSize);
					pDlg->m_pSerialPort->WriteToPort(szTXBuffer, nWriteSize);

					if (nSize > BUFFER_SIZE2)
					{
						memmove(szRXBuffer, &szRXBuffer[BUFFER_SIZE4], nSize-BUFFER_SIZE4);
						nSize-=BUFFER_SIZE4;
					}

					if ((GetTickCount() - pDlg->m_dwLastTickCount) > SIMPLE_OPERATION_TO)
					{
						// Exit
						pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE);
						pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE;
						pDlg->m_nState = FSM_UPDATE_WAIT;
					}					
				}
			}
			break;
			case FSM_UPDATE_CONF_DL:
			{
				nReadSize = pDlg->ReadRxBuffer(&szRXBuffer[nSize], BUFFER_SIZE);
				nSize += nReadSize;
				if (memmem(UPD_PROG_ASK, strlen(UPD_PROG_ASK), (char *)szRXBuffer, nSize) != NULL)
				{
					pDlg->m_nState = FSM_UPDATE_CONF_TRANS;
					pDlg->m_nSubState = 0;
					pDlg->m_dwLastTickCount = GetTickCount();

					nSize = 0;
					nWriteSize = strlen(YES_PASS);
					memcpy(szTXBuffer, YES_PASS, nWriteSize);
					pDlg->m_pSerialPort->WriteToPort(szTXBuffer, nWriteSize);
				}
				else
				{
					if ((GetTickCount() - pDlg->m_dwLastTickCount) > SIMPLE_OPERATION_TO)
					{
						// Exit
						pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE);
						pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE;
						pDlg->m_nState = FSM_UPDATE_WAIT;
					}			
				}
			}
			break;
			case FSM_UPDATE_CONF_TRANS:
			{
				nReadSize = pDlg->ReadRxBuffer(&szRXBuffer[nSize], BUFFER_SIZE);
				nSize += nReadSize;
				if (memmem(TRANSMIT_FILE, strlen(TRANSMIT_FILE), (char *)szRXBuffer, nSize) != NULL)
				{
					pDlg->m_nState = FSM_UPDATE_PROCESS;
					pDlg->m_nSubState = 0;
					pDlg->m_dwLastTickCount = GetTickCount();

					nSize = 0;
					
					pLDRFile = new CFile(pDlg->m_csFirmWarePath, CFile::modeRead);
					if (pLDRFile)
					{
						nFileSize = pLDRFile->GetLength();
						nFileBufferSize = pLDRFile->Read(szFileBuffer, FILE_BUFFER_SIZE);

						pDlg->m_ProgressGuage.SetRange32(0, (int)nFileSize);
						pDlg->m_ProgressGuage.SetPos(0);

						nLineIndex = 1;

						memset(szTXBuffer, 0, BUFFER_SIZE);
						sscanf_s((char *)szFileBuffer, _T("%s\r\n"), (char *)szTXBuffer, BUFFER_SIZE);
						nWriteSize = strlen((char *)szTXBuffer);
						pDlg->m_pSerialPort->WriteToPort(szTXBuffer, nWriteSize);

						if (nFileBufferSize >= (nWriteSize+2))
						{
							memmove(szFileBuffer, &szFileBuffer[nWriteSize+2], nFileBufferSize-(nWriteSize+2));
							nFileBufferSize-=(nWriteSize+2);
						}
						else nFileBufferSize=0;
						

						sprintf_s((char *)szMustRXBuffer, BUFFER_SIZE, LINE_NUMBER, nLineIndex);
						nMustRxSize = strlen((char *)szMustRXBuffer);
						nCurrSize = nWriteSize+2;
						nPersent = 0;
					}
					else
					{
						// exit
						pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__FILE_CANT_OPEN);
						pDlg->m_nState = FSM_UPDATE_WAIT;
					}
				}
				else
				{
					if ((GetTickCount() - pDlg->m_dwLastTickCount) > SIMPLE_OPERATION_TO)
					{
						// Exit
						pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE);
						pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE;
						pDlg->m_nState = FSM_UPDATE_WAIT;
					}			
				}			
			}
			break;
			case FSM_UPDATE_PROCESS:
			{
				nReadSize = pDlg->ReadRxBuffer(&szRXBuffer[nSize], BUFFER_SIZE);
				nSize += nReadSize;

				if (nCurrSize < nFileSize)
				{
					if (memmem((char *)szMustRXBuffer, nMustRxSize, (char *)szRXBuffer, nSize) != NULL)
					{
						if (nFileBufferSize < FILE_BUFFER_SIZE2)
						{
							nReadSize = pLDRFile->Read(&szFileBuffer[nFileBufferSize], FILE_BUFFER_SIZE2);	
							nFileBufferSize += nReadSize;
						}

						pDlg->m_dwLastTickCount = GetTickCount();
						nSize = 0;
						nLineIndex++;
						memset(szTXBuffer, 0, BUFFER_SIZE);
						sscanf_s((char *)szFileBuffer, _T("%s\r\n"), (char *)szTXBuffer, BUFFER_SIZE);
						nWriteSize = strlen((char *)szTXBuffer);
						pDlg->m_pSerialPort->WriteToPort(szTXBuffer, nWriteSize);			

						if (nFileBufferSize >= (nWriteSize+2))
						{
							memmove(szFileBuffer, &szFileBuffer[nWriteSize+2], nFileBufferSize-(nWriteSize+2));
							nFileBufferSize-=(nWriteSize+2);
						}
						else 
						{
							nFileBufferSize=0;
						}

						sprintf_s((char *)szMustRXBuffer, BUFFER_SIZE, LINE_NUMBER, nLineIndex);
						nMustRxSize = strlen((char *)szMustRXBuffer);
						nCurrSize += nWriteSize+2;
						pDlg->m_ProgressGuage.SetPos((int)nCurrSize);
						dPersent = 100*(double)nCurrSize/(double)nFileSize;
						if (nPersent != (int)dPersent)
						{
							nPersent = (int)dPersent;
							csProcessMsg.Format(UPDATE_FIRMWARE_PROCESS, nPersent);
							pDlg->SetDlgItemText(IDC_STATIC_PROCESS, csProcessMsg);
						}
					}
					else
					{
						if ((GetTickCount() - pDlg->m_dwLastTickCount) > SIMPLE_OPERATION_TO)
						{
							// Exit
							pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE);
							pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE;
							pDlg->m_nState = FSM_UPDATE_WAIT;

							if (pLDRFile)
							{
								pLDRFile->Close();
								delete pLDRFile;
							}
						}								
					}
				}
				else
				{
					sprintf_s((char *)szMustRXBuffer, BUFFER_SIZE, LINE_NUMBER, nLineIndex);
					strcat_s((char *)szMustRXBuffer, BUFFER_SIZE, END_OF_FILE);
					strcat_s((char *)szMustRXBuffer, BUFFER_SIZE, INFO);
          strcat_s((char *)szMustRXBuffer, BUFFER_SIZE, TOT_BYTES_RX);
          strcat_s((char *)szMustRXBuffer, BUFFER_SIZE, HOW_MUCH_WRITE);          
          strcat_s((char *)szMustRXBuffer, BUFFER_SIZE, STANDARD_CHECKSUM);          
          strcat_s((char *)szMustRXBuffer, BUFFER_SIZE, UPD_PROG_DONE);


					nScanRes = sscanf_s((char *)szRXBuffer,
															(char *)szMustRXBuffer,
                              &nbytes_rcv,
                              &nkb_rcv,
                              &npage_trm,
                              &nbytes_trm,
                              &nkb_trm,
                              &dwstdcrc,
															BUFFER_SIZE);
					if (nScanRes == 6)
          {
						// OK						
						pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__SUCCESS);
						pDlg->m_nState = FSM_UPDATE_WAIT;						

						pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__SUCCESS;
						if (pLDRFile)
						{
							pLDRFile->Close();
							delete pLDRFile;
						}
						PostMessage(pDlg->m_hWnd, WM_CLOSE, 0, 0);
          }
					else
					{
						if ((GetTickCount() - pDlg->m_dwLastTickCount) > SIMPLE_OPERATION_TO)
						{
							// Exit
							pDlg->SetDlgItemText(IDC_STATIC_PROCESS, UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE);
							pDlg->m_dwResult = UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE;
							pDlg->m_nState = FSM_UPDATE_WAIT;							

							if (pLDRFile)
							{
								pLDRFile->Close();
								delete pLDRFile;
							}
						}														
					}
				}
			}
			break;
			default:
			{
				pDlg->m_nState = FSM_UPDATE_WAIT;
			}
		}
	}

	if (pLDRFile)
	{
		pLDRFile->Close();
		delete pLDRFile;
	}
	CloseHandle(hFakeEvent);
	return 0;
}

int	CUpdateFirmWare::FlushRxBuffer(void)
{
	EnterCriticalSection(m_pCS);
	if (m_nCounter > 0)
	{
		m_nCounter = 0;
	}
	LeaveCriticalSection(m_pCS);
	return 0;
}

int	CUpdateFirmWare::ReadRxBuffer(BYTE szBuffer[], int nSize)
{
	int nReadSize=0;

	EnterCriticalSection(m_pCS);
	if (m_nCounter > 0)
	{
		if (nSize >= m_nCounter)
		{
			memcpy(szBuffer, m_bRxBuff, m_nCounter);
			nReadSize = m_nCounter;
			m_nCounter = 0;
		}
		else
		{
			memcpy(szBuffer, m_bRxBuff, nSize);
			nReadSize = nSize;
			m_nCounter -= nSize;
		}		
	}
	LeaveCriticalSection(m_pCS);

	return nReadSize;
}

LRESULT CUpdateFirmWare::OnRXChar( WPARAM wParam, LPARAM lParam ) 
{
	BYTE ch = (BYTE)wParam;
	if (m_nPort == lParam)
	{
		EnterCriticalSection(m_pCS);
		if (m_nCounter < BUFFER_SIZE)
		{
			m_bRxBuff[m_nCounter++] = ch;		
		}
		LeaveCriticalSection(m_pCS);
	}

	return 0L;
}
void CUpdateFirmWare::OnClose()
{
	if (m_pThreadProc)
	{
		m_pThreadProc->SuspendThread();
		delete m_pThreadProc;m_pThreadProc=NULL;
	}

  if (m_pSerialPort)
  {
    m_pSerialPort->StopMonitoring();
    delete m_pSerialPort;
    m_pSerialPort = NULL;
  }

	if (m_pCS)
	{
		DeleteCriticalSection(m_pCS);
		delete m_pCS;m_pCS = NULL;
	}

	CDialog::OnClose();
}

void CUpdateFirmWare::OnBnClickedCancel()
{
	if (m_pThreadProc)
	{
		m_pThreadProc->SuspendThread();
		delete m_pThreadProc;m_pThreadProc=NULL;
	}

  if (m_pSerialPort)
  {
    m_pSerialPort->StopMonitoring();
    delete m_pSerialPort;
    m_pSerialPort = NULL;
  }

	if (m_pCS)
	{
		DeleteCriticalSection(m_pCS);
		delete m_pCS;m_pCS = NULL;
	}

	OnCancel();
}

