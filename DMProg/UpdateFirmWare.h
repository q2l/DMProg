#pragma once

#include "SerialPort.h"

#include "afxcmn.h"

#define FSM_UPDATE_WAIT					0
#define FSM_UPDATE_INIT					1
#define FSM_UPDATE_START_DL			2
#define FSM_UPDATE_CONF_DL			3
#define FSM_UPDATE_CONF_TRANS		4
#define FSM_UPDATE_PROCESS			5

#define PROG_INIT_CH						0x3E
#define PROG_INIT_PACKET_SIZE		6

#define BUFFER_SIZE							4096
#define BUFFER_SIZE2						(BUFFER_SIZE/2)
#define BUFFER_SIZE4						(BUFFER_SIZE/4)

#define FILE_BUFFER_SIZE					4096
#define FILE_BUFFER_SIZE2				(FILE_BUFFER_SIZE/2)
#define FILE_BUFFER_SIZE4				(FILE_BUFFER_SIZE/4)


#define ENTER_PASS			_T("\r")
#define NO_PASS					_T("n\r")
#define YES_PASS				_T("y\r")
#define DL_PASS					_T("dl\r")

#define ERROR_COMMAND		_T("UNKNOWN COMMAND\n\r")
#define UPD_PROG_ASK		_T("Do you want to update programm [y/n]\n\r")
#define TRANSMIT_FILE		_T("\n\rTransmit file\n\r")
#define UPD_PROG_DONE		_T("\n\rUpdate programm done!\n\r")
#define UPD_PROG_CANCEL	_T("\n\rUpdate programm cancel!\n\r")

#define LINE_NUMBER             _T("\n\rok line = %05d")
#define LINE_MUMBER_SIZE        18

#define END_OF_FILE					_T("   End of file\n\r")
#define INFO                    _T("\n\n\r========= I N F O R M A T I O N =========")
#define TOT_BYTES_RX		_T("\n\r   - total hex bytes received  %d  (%d KB)")
#define HOW_MUCH_WRITE		_T("\n\r   - pages write               %d\n\r   - bytes write               %d  (%d KB)\n\r")
#define CRC32_VALUE			_T("\n\r   - CRC32                     0x%08X")
#define STANDARD_CHECKSUM	_T("\n\r   - Standard checksum         0x%08X")
#define	ELAPS_TIME			_T("\n\r   - elapsed time            %4d.%01d sec")



#define UPDATE_FIRMWARE_ERROR__FILE_NOT_FOUND				_T("Файл не найден!")
#define UPDATE_FIRMWARE_ERROR__DEVICE_NOT_RESPONSE	_T("Модем не отвечает!")
#define UPDATE_FIRMWARE_ERROR__FILE_CANT_OPEN				_T("Файл не может быть открыт для чтения!")
#define UPDATE_FIRMWARE_ERROR__SUCCESS							_T("Программирование успешно завершено!")
#define UPDATE_FIRMWARE_ERROR__POWERON							_T("Включите модем!")

#define UPDATE_FIRMWARE_PROCESS											_T("Обновление %d%% ...")


#define WAIT_TO_START_TO						60000
#define SIMPLE_OPERATION_TO					3000


#define UPDATE_FIRMWARE_ERROR_ID__SUCCESS									0
#define UPDATE_FIRMWARE_ERROR_ID__CANCEL									1
#define UPDATE_FIRMWARE_ERROR_ID__FILE_NOT_FOUND					2
#define UPDATE_FIRMWARE_ERROR_ID__DEVICE_NOT_RESPONSE			3
#define UPDATE_FIRMWARE_ERROR_ID__FILE_CANT_OPEN					4




char * memmem(char *m1, int size1, char *m2, int size2);

// CUpdateFirmWare dialog

class CUpdateFirmWare : public CDialog
{
	DECLARE_DYNAMIC(CUpdateFirmWare)

public:

	CString						m_csFirmWarePath;
	OPENFILENAME			m_ofnParam;
	int								m_nPort;
	CSerialPort				*m_pSerialPort;
	CWinThread				*m_pThreadProc;
	DWORD							m_dwTimeOut;
	int								m_nState;
	int								m_nSubState;
	BYTE							m_bRxBuff[BUFFER_SIZE];
	int								m_nCounter;
	CRITICAL_SECTION	*m_pCS;
	DWORD							m_dwLastTickCount;
	DWORD							m_dwResult;

	CUpdateFirmWare(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdateFirmWare();
	virtual  BOOL OnInitDialog();

	virtual BOOL PreTranslateMessage(MSG* pMsg);


	LRESULT OnRXChar( WPARAM wParam, LPARAM lParam );
	int	ReadRxBuffer(BYTE szBuffer[], int nSize);
	int	FlushRxBuffer(void);

// Dialog Data
	enum { IDD = IDD_DIALOG_UPDATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenDialog();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnClose();
	CProgressCtrl m_ProgressGuage;
	afx_msg void OnBnClickedCancel();
};
