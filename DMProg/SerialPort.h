// SerialPort.h: interface for the CSerialPort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORT_H__677CBD03_CF57_48E7_99E2_F249FA569359__INCLUDED_)
#define AFX_SERIALPORT_H__677CBD03_CF57_48E7_99E2_F249FA569359__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_COMM_BREAK_DETECTED		WM_USER+1	// A break was detected on input.
#define WM_COMM_CTS_DETECTED		WM_USER+2	// The CTS (clear-to-send) signal changed state.
#define WM_COMM_DSR_DETECTED		WM_USER+3	// The DSR (data-set-ready) signal changed state.
#define WM_COMM_ERR_DETECTED		WM_USER+4	// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
#define WM_COMM_RING_DETECTED		WM_USER+5	// A ring indicator was detected.
#define WM_COMM_RLSD_DETECTED		WM_USER+6	// The RLSD (receive-line-signal-detect) signal changed state.
#define WM_COMM_RXCHAR			WM_USER+7	// A character was received and placed in the input buffer.
#define WM_COMM_RXFLAG_DETECTED		WM_USER+8	// The event character was received and placed in the input buffer.
#define WM_COMM_TXEMPTY_DETECTED	WM_USER+9	// The last character in the output buffer was sent.

#define WM_COMM_OR_FILE_ERROR           WM_USER+10      // Fatal Error (Send from ProcessErrorMessage())

class CSerialPort
{
public:
	// contruction and destruction
	CSerialPort();
	virtual		~CSerialPort();

	// port initialisation
	bool		InitPort(HWND hwnd,
                                 unsigned int portnr = 1,
                                 unsigned int baud = 4800,
                                 char parity = 'N',
                                 unsigned int databits = 8,
                                 unsigned int stopsbits = 1,
                                 DWORD dwCommEvents = EV_RXCHAR | EV_TXEMPTY,
                                 unsigned int nBufferSize = 1024);

	// start/stop comm watching
	bool		StartMonitoring();
	bool		RestartMonitoring();
	bool		StopMonitoring();

	DWORD		GetWriteBufferSize();
	DWORD		GetCommEvents();
	DCB			GetDCB();

	void		WriteToPort(LPVOID string, int nCount);
        DWORD           GetPortNumber() const;
		HANDLE      GetHandle() const{return m_hComm;};
	bool		ClosePort(void);


	char				m_LastError[1024];

protected:
	// protected memberfunctions
	void		ProcessErrorMessage(char* ErrorText);
	static unsigned long _stdcall	CommThread(LPVOID pParam);
	static void	ReceiveChar(CSerialPort* port, COMSTAT comstat);
	static void	WriteChar(CSerialPort* port);

	// thread
	HANDLE  			m_Thread;
        DWORD                           m_ThreadID;

	// synchronisation objects
	CRITICAL_SECTION        	m_csCommunicationSync;
	bool				m_bThreadAlive;

	// handles
	HANDLE				m_hShutdownEvent;
	HANDLE				m_hComm;
	HANDLE				m_hWriteEvent;

	// Event array. 
	// One element is used for each event. There are two event handles for each port.
	// A Write event and a receive character event which is located in the overlapped structure (m_ov.hEvent).
	// There is a general shutdown when the port is closed. 
	HANDLE				m_hEventArray[3];

	// structures
	OVERLAPPED			m_ov;
	COMMTIMEOUTS	        	m_CommTimeouts;
	DCB				m_dcb;

	// window handle
	HWND                            m_hWnd;

	// misc
	DWORD   		       	m_nPortNr;
	unsigned char*		        m_szWriteBuffer;
	DWORD				m_dwCommEvents;
	DWORD				m_nWriteBufferSize;
	DWORD				m_nWriteCount;
};


#endif // !defined(AFX_SERIALPORT_H__677CBD03_CF57_48E7_99E2_F249FA569359__INCLUDED_)
