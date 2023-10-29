// ConsoleCommands.c
// This is where you add commands:
//		1. Add a protoype
//			static eCommandResult_T ConsoleCommandVer(const char buffer[]);
//		2. Add the command to mConsoleCommandTable
//		    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
//		3. Implement the function, using ConsoleReceiveParam<Type> to get the parameters from the buffer.

#include <string.h>
#include "consoleCommands.h"
#include "console.h"
#include "consoleIo.h"
#include "version.h"

#define IGNORE_UNUSED_VARIABLE(x)     if ( &x == &x ) {}

static eCommandResult_T ConsoleCommandComment(const char buffer[]);
static eCommandResult_T ConsoleCommandVer(const char buffer[]);
static eCommandResult_T ConsoleCommandHelp(const char buffer[]);
static eCommandResult_T ConsoleCommandParamExampleInt16(const char buffer[]);
static eCommandResult_T ConsoleCommandParamExampleHexUint16(const char buffer[]);

static eCommandResult_T ConsoleCommandToggleIO(const char buffer[]);
static eCommandResult_T ConsoleCommandReadIO(const char buffer[]);
static eCommandResult_T ConsoleCommandWriteIO(const char buffer[]);

__weak void ToggleIOCommandCallback(char port, int16_t io)
{
    ConsoleIoSendString("Parameter are ");
    ConsoleSendParamInt16(port);
    ConsoleIoSendString(" ");
    ConsoleSendParamInt16(io);
    ConsoleIoSendString(STR_ENDLINE);
}

__weak void ReadIOCommandCallback(char port, int16_t io)
{
    ConsoleIoSendString("Parameter are ");
    ConsoleSendParamInt16(port);
    ConsoleIoSendString(" ");
    ConsoleSendParamInt16(io);
    ConsoleIoSendString(STR_ENDLINE);
}

__weak void WriteIOCommandCallback(char port, int16_t io, int16_t value)
{
    ConsoleIoSendString("Parameter are ");
    ConsoleSendParamInt16(port);
    ConsoleIoSendString(" ");
    ConsoleSendParamInt16(io);
    ConsoleIoSendString(" ");
    ConsoleSendParamInt16(value);
    ConsoleIoSendString(STR_ENDLINE);
}

static const sConsoleCommandTable_T mConsoleCommandTable[] =
{
    {";", &ConsoleCommandComment, HELP("Comment! You do need a space after the semicolon. ")},
    {"help", &ConsoleCommandHelp, HELP("Lists the commands available")},
    {"ver", &ConsoleCommandVer, HELP("Get the version string")},
    {"int", &ConsoleCommandParamExampleInt16, HELP("How to get a signed int16 from params list: int -321")},
    {"u16h", &ConsoleCommandParamExampleHexUint16, HELP("How to get a hex u16 from the params list: u16h aB12")},
    {"tio", &ConsoleCommandToggleIO, HELP("How to toggle the I/O pin 15 port B: tio b 15")},
    {"rio", &ConsoleCommandReadIO, HELP("Read from an I/O pin 15 port B: rio b 15")},
    {"wio", &ConsoleCommandWriteIO, HELP("Write to an I/O pin 15 port B: wio b 15 1")},

	CONSOLE_COMMAND_TABLE_END // must be LAST
};

static eCommandResult_T ConsoleCommandComment(const char buffer[])
{
	// do nothing
	IGNORE_UNUSED_VARIABLE(buffer);
	return COMMAND_SUCCESS;
}

static eCommandResult_T ConsoleCommandHelp(const char buffer[])
{
	uint32_t i;
	uint32_t tableLength;
	eCommandResult_T result = COMMAND_SUCCESS;

    IGNORE_UNUSED_VARIABLE(buffer);

	tableLength = sizeof(mConsoleCommandTable) / sizeof(mConsoleCommandTable[0]);
	for ( i = 0u ; i < tableLength - 1u ; i++ )
	{
		ConsoleIoSendString(mConsoleCommandTable[i].name);
#if CONSOLE_COMMAND_MAX_HELP_LENGTH > 0
		ConsoleIoSendString(" : ");
		ConsoleIoSendString(mConsoleCommandTable[i].help);
#endif // CONSOLE_COMMAND_MAX_HELP_LENGTH > 0
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandParamExampleInt16(const char buffer[])
{
	int16_t parameterInt;
	eCommandResult_T result;
	result = ConsoleReceiveParamInt16(buffer, 1, &parameterInt);
	if ( COMMAND_SUCCESS == result )
	{
		ConsoleIoSendString("Parameter is ");
		ConsoleSendParamInt16(parameterInt);
		ConsoleIoSendString(" (0x");
		ConsoleSendParamHexUint16((uint16_t)parameterInt);
		ConsoleIoSendString(")");
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}
static eCommandResult_T ConsoleCommandParamExampleHexUint16(const char buffer[])
{
	uint16_t parameterUint16;
	eCommandResult_T result;
	result = ConsoleReceiveParamHexUint16(buffer, 1, &parameterUint16);
	if ( COMMAND_SUCCESS == result )
	{
		ConsoleIoSendString("Parameter is 0x");
		ConsoleSendParamHexUint16(parameterUint16);
		ConsoleIoSendString(STR_ENDLINE);
	}
	return result;
}

static eCommandResult_T ConsoleCommandVer(const char buffer[])
{
	eCommandResult_T result = COMMAND_SUCCESS;

    IGNORE_UNUSED_VARIABLE(buffer);

	ConsoleIoSendString(VERSION_STRING);
	ConsoleIoSendString(STR_ENDLINE);
	return result;
}

static eCommandResult_T ConsoleCommandToggleIO(const char buffer[]) {
    char port;
    int16_t io;
    eCommandResult_T result;
    do {
        result = ConsoleReceiveParamChar(buffer, 1, &port);
        if(result != COMMAND_SUCCESS)
            break;
        result = ConsoleReceiveParamInt16(buffer, 2, &io);
        if(result != COMMAND_SUCCESS)
            break;

        // making sure i/o doesn't exceed 15 and port is between 'a' and 'h'
        if((io <= 15) && (port >= 97 && port <= 104)) {
            ToggleIOCommandCallback(port, io);
            ConsoleIoSendString(STR_ENDLINE); // TODO: send VT100 code to clear terminal of previous command?
        } else {
            result = COMMAND_PARAMETER_ERROR;
        }

    } while(0);

    return result;
}

static eCommandResult_T ConsoleCommandReadIO(const char buffer[]) {
    char port;
    int16_t io;
    eCommandResult_T result;

    do {
        result = ConsoleReceiveParamChar(buffer, 1, &port);
        if(result != COMMAND_SUCCESS)
            break;
        result = ConsoleReceiveParamInt16(buffer, 2, &io);
        if(result != COMMAND_SUCCESS)
            break;

        // making sure i/o doesn't exceed 15 and port is between 'a' and 'h'
        if((io <= 15) && (port >= 97 && port <= 104)) {
            ReadIOCommandCallback(port, io);
            ConsoleIoSendString(STR_ENDLINE); // TODO: send VT100 code to clear terminal of previous command?
        } else {
            result = COMMAND_PARAMETER_ERROR;
        }
    } while(0);

    return result;
}
static eCommandResult_T ConsoleCommandWriteIO(const char buffer[]) {
    char port;
    int16_t io;
    int16_t value;

    eCommandResult_T result;

    do {
        result = ConsoleReceiveParamChar(buffer, 1, &port);
        if(result != COMMAND_SUCCESS)
            break;
        result = ConsoleReceiveParamInt16(buffer, 2, &io);
        if(result != COMMAND_SUCCESS)
            break;
        result = ConsoleReceiveParamInt16(buffer, 3, &value);
        if(result != COMMAND_SUCCESS)
            break;

        // making sure i/o doesn't exceed 15 and port is between 'a' and 'h'
        if((io <= 15) && (port >= 97 && port <= 104) && (value == 0 || value == 1)) {
            WriteIOCommandCallback(port, io, value);
            ConsoleIoSendString(STR_ENDLINE); // TODO: send VT100 code to clear terminal of previous command?
        } else {
            result = COMMAND_PARAMETER_ERROR;
        }

    } while(0);

    return result;
}


const sConsoleCommandTable_T* ConsoleCommandsGetTable(void)
{
	return (mConsoleCommandTable);
}


