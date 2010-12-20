
//
// This header consists any params for 
//

#pragma once

#define MODEM_NAME						"DM9600M"
#define MODEM_VERSION_MAJOR		1
#define MODEM_VERSION_MINOR		1

#define VERSION_NAME				_T("DM9600M v1.01")
#define VERSION_NAME2				"DM9600M v1.01"
#define PARAM_COMM_PORT				_T("Port")
#define PARAM_UPDATE_ACCESS		_T("UpdateAccess")

#define BYTE_RANGE_MIN				0
#define BYTE_RANGE_MAX				255

#define DEF_PARAM_ADDR_SYSTEM		1
#define DEF_PARAM_ADDR_MODEM		1
#define DEF_PARAM_ADDR_SENDER		1
#define DEF_PARAM_ADDR_RECEIVER	1

#define ADDR_TX_MIN							0
#define ADDR_TX_MAX							3
#define DEF_PARAM_ADDR_TX				0

#define ADDR_RX_MIN							0
#define ADDR_RX_MAX							4
#define DEF_PARAM_ADDR_RX				0

#define SQL_MIN									0
#define SQL_MAX									1
#define DEF_PARAM_SQL						0

#define TXMODE_MIN							0
#define TXMODE_MAX							2
#define DEF_PARAM_TXMODE				0

#define BAUD_RATE_MIN				0
#define BAUD_RATE_MAX				8
#define BAUD_RATE_DEFAULT			5

#define DATA_BITS_MIN				0
#define DATA_BITS_MAX				3
#define DATA_BITS_DEFAULT			3

#define STOP_BITS_MIN				0
#define STOP_BITS_MAX				1
#define STOP_BITS_DEFAULT			0

#define PARITY_MIN					0
#define PARITY_MAX					2
#define PARITY_DEFAULT				0

#define SPECSYM_MIN					0
#define SPECSYM_MAX					255
#define SPECSYM_DEFAULT				10

#define BUFFER_SIZE_MIN				1
#define BUFFER_SIZE_MAX				510
#define BUFFER_SIZE_DEFAULT			510
#define BUFFER_SIZE_STEP			2

#define TO_LAST_BYTE_MIN			10
#define TO_LAST_BYTE_MAX			2550
#define TO_LAST_BYTE_DEFAULT		10
#define TO_LAST_BYTE_STEP			10

#define REMOUT_CONTROL_MIN			0
#define REMOUT_CONTROL_MAX			1
#define REMOUT_CONTROL_DEFAULT		1

#define LOCAL_ECHO_MIN				0
#define LOCAL_ECHO_MAX				1
#define LOCAL_ECHO_DEFAULT			0

#define TO_PRETRANS_MIN				0
#define TO_PRETRANS_MAX				2550
#define TO_PRETRANS_DEFAULT			200
#define TO_PRETRANS_STEP			10

#define TO_POSTTRANS_MIN			0
#define TO_POSTTRANS_MAX			2550
#define TO_POSTTRANS_DEFAULT		0
#define TO_POSTTRANS_STEP			10


#define FSM_WAIT					0
#define FSM_READ					1
#define FSM_WRITE					2

#define FSM_READ_WAIT_START		0
#define FSM_READ_READ_VERSION	1
#define FSM_READ_READ_PROCESS	2

#define FSM_WRITE_WAIT_START		0
#define FSM_WRITE_WRITE_PROCESS	1

#define FSMS_BINARY_CMD_NULL		0x00
#define FSMS_BINARY_CMD_READ_BASE	0x01
#define FSMS_BINARY_CMD_WRITE_BASE	0x02
#define FSMS_BINARY_CMD_READ_EX		0x05
#define FSMS_BINARY_CMD_WRITE_EX	0x06

#define FSMS_BINARY_CMD_SET_MODE	0x10

#define FSMS_BINARY_CMD_TEST_TRANS	0x13
#define FSMS_BINARY_CMD_TEST_GAIN	0x14

#define FSMS_BINARY_CMD_INFO		0x20

#define FSMS_BINARY_CMD_NAK			0xAA
#define FSMS_BINARY_CMD_ACK			0x55


#define BINARY_PACKET_HEADER		0x40
#define BINARY_PACKET_RETRY			3
#define BINARY_PACKET_TIMEOUT		100
#define BINARY_INFO_PACKET_SIZE		64	
#define NAME_SIZE					16
#define BINARY_PACKET_MAX_SIZE		128
#define BINARY_PACKET_MIN_SIZE		4
#define NUM_OF_REGS					100		/* number of all registers of modem. */

#define TIMEOUT_READ_ACK			1100
#define COUNT_READ_INFO_RETRY		3

