#include <monohome.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

#define MODEM_NAME "SIM800 2G modem"
#define MODEM_TERMINAL "/dev/ttyMT3"

#define SMS_END 26

void CModem::Init()
{
	modemFd = open(MODEM_TERMINAL, O_RDWR);
	
	if(modemFd < 0)
	{
		printf("Failed to open %s\n", MODEM_TERMINAL);
		return;
	}
	
	// Assign so-called raw mode for terminal.
	termios tio;
	
	tcgetattr(modemFd, &tio);
    tio.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    tio.c_oflag &= ~(OPOST);
    tio.c_cflag |= (CS8);
    tio.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	tcsetattr(modemFd, TCSAFLUSH, &tio);
	
	char atBuf[16];
	// Check modem presence
	SendAT("AT\r\n", 250);
	GetATResponse((char*)&atBuf, sizeof(atBuf));
	
	if(!CheckATStatus((char*)&atBuf))
	{
		printf("Failed to initialize modem: Modem isn't anwered OK\n");
		printf("Modem response: %s\n", &atBuf);
		return;
	}
	
	LOG("AT = OK, ready to operate\n");
}

void CModem::ExitFlightMode()
{
	
}

bool CModem::CheckATStatus(char* response)
{
	return strstr(response, "OK");
}

CModem::CModem()
{
	LOG("Initializing modem...\n");
	LOGF("Modem: %s\n", MODEM_NAME);
	LOGF("Terminal: %s\n", MODEM_TERMINAL);
	
	Init();
	ExitFlightMode();
}

CModem::~CModem()
{
	close(modemFd);
}

void CModem::SendAT(char* command, int waitTime)
{
	int result = write(modemFd, command, strlen(command));

	if(!result)
		LOGF("SendAT failed: %i\n", errno);

		usleep(1000 * waitTime);
}

char* CModem::GetATResponse(char* buf, int maxLen)
{
	pollfd pfd;
	pfd.fd = modemFd;
	pfd.events = POLLIN;
	
	memset(buf, 0, maxLen);
	int ev = poll(&pfd, 1, 2000);
	
	if(ev)
	{
		int num = read(modemFd, buf, maxLen);
	}
	else
	{
		LOG("AT Receive: Modem not responding...\n");
	}
}

bool CModem::IsReady()
{
	return readyState;
}

char* CModem::GetIMEI()
{
	return (char*)&imei;
}

char* CModem::GetIMSI()
{
	
}

EModemDialState CModem::GetDialState()
{
	char atBuf[64];
	
	SendAT("AT+CPAS\r\n", 250);
	GetATResponse((char*)&atBuf, sizeof(atBuf));
	LOGF("AT+CPAS: %s\n", &atBuf);
}

void CModem::Dial(char* number)
{
	if(strlen(number) > 32)
		return;
	
	char buf[64]; 
	char atResponse[64];
	sprintf((char*)&buf, "ATD%s;\r\n", number);
	LOGF("Dialing %s\n", buf);
	
	SendAT(buf, 250);
	GetATResponse((char*)&atResponse, sizeof(atResponse));
	
	LOGF("Dial response: %s\n", &atResponse);
}

void CModem::Hang()
{
	char atBuf[64];
	
	SendAT("ATH\r\n", 250);
	GetATResponse((char*)&atBuf, sizeof(atBuf));
	LOGF("ATH: %s\n", &atBuf);
}

void CModem::SendSMS(char* number, char* text)
{
	char atBuf[64];
	
	//SendAT("AT+CSQ\r\n", 250);
	//GetATResponse((char*)&atBuf, sizeof(atBuf));
	//LOGF("CMGF status: %s\n", &atBuf);
	
	SendAT("ATA\r\n", 250);
	GetATResponse((char*)&atBuf, sizeof(atBuf));
	LOGF("ATA: %s\n", &atBuf);
	
}
