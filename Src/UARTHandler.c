#include <string.h>
#include "global.h"
#include "UARTHelper.h"
#include "UARTHandler.h"


DaughterBoard * getDaughterBoard(UartInterface * uartInterface, int * whichBoard) {
	*whichBoard = -1;
	for (int i = 0; i < 7; i++) {
		if (namedInterface.daughterBoards[i].uartInterface == uartInterface) {
			*whichBoard = i;
			return &namedInterface.daughterBoards[i];
		}
	}
	return NULL;
}

void registerTestBoard(DaughterBoard * daughterBoard, int whichTB, char * content, int offset) {
	daughterBoard->isBoardInserted[whichTB] = true;
	strncpy(daughterBoard->uuid[whichTB], content + offset, 36);
}

void unregisterTestBoard(DaughterBoard * daughterBoard, int whichTB) {
	daughterBoard->isBoardInserted[whichTB] = false;
	memset(daughterBoard->uuid[whichTB], 0, 37);
}

void processDaughterBoardResponse(DaughterBoard * daughterBoard, int whichBoard, char * response) {
	char commandCode = response[3];
	int whichTB = response[1] - 48;
	
	switch(commandCode) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
			sendToUART(namedInterface.pcUART, "#%d%s\n", whichBoard, response);
		case 'f':
			registerTestBoard(daughterBoard, whichTB, response, 5);
			break;
		case 'g':
			unregisterTestBoard(daughterBoard, whichTB);
			break;
		default:
			break;
	}
	
}

int isCorrectCommandFromPC(char * command) {
	return strlen(command) == 9 &&
				 (*(command+0) == '$') &&
				 (*(command+1) == '0' || *(command+1) == '1') &&
				 (*(command+2) == '$') &&
				 (*(command+3) == '0' || *(command+3) == '1') &&
				 (*(command+4) == '$') &&
				 (*(command+6) == '$') &&
				 (*(command+8) == '$');
	
}

void processPCCommand(char * command) {
	char commandCode = command[5];
	int whichBoard = command[1] - 48;
	int whichTB = command[3] - 48; 
	
	switch(commandCode) {
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
			if (true) { //namedInterface.daughterBoards[whichBoard].isBoardInserted[whichTB]) {
				sendToUART(namedInterface.daughterBoards[whichBoard].uartInterface, "%s\n", command + 2);
			} else {
				sendToUART(namedInterface.pcUART, "#NOTFOUND#%d#%d#\n", whichBoard, whichTB);
			}
			break;
		case 'f':
			break;
		default:
			break;
	}
}

void uartReceiverCallback(UartInterface * uartInterface, char * content) {
	int whichBoard = -1;
	DaughterBoard * daughterBoard = getDaughterBoard(uartInterface, &whichBoard);
	
	if (daughterBoard != NULL) {
		int commandLength = strlen(content);
		bool isPingCommand = 
			commandLength > 9 &&
			(content[0] == '$') && 
			(content[1] == 'P') &&
			(content[2] == 'I') &&
			(content[3] == 'N') &&
			(content[4] == 'G') &&
			(content[5] == '$') &&
			(content[6] == '0' || content[6] == '1') &&
			(content[7] == '$') &&
			(content[8] == '0' || content[8] == '1') &&
			(content[commandLength-1]) == '$';
		
		bool isResponse = 
			commandLength > 3 &&
			(content[0] == '#') && 
			(content[1] == '0' || content[1] == '1') &&
		  (content[2] == '#') && 
			(content[commandLength-1] == '#');
		
		if (isPingCommand) {
			int whichTB = content[6] - 48;
			bool isInserted = content[8]  == '1';
			namedInterface.daughterBoards[whichBoard].isBoardInserted[whichTB] = isInserted;
			
			if (commandLength == 47) {
				registerTestBoard(daughterBoard, whichTB, content, 10);
			}
			
			sendToUART(uartInterface, "#PONG#\n");
			
		} else if (isResponse) {
			processDaughterBoardResponse(daughterBoard, whichBoard, content);
		}
	} else if (uartInterface == namedInterface.pcUART) {
		
		if (isCorrectCommandFromPC(content)) {
			processPCCommand(content);
		} else {
			debugMessage("Received from PC: %s\n", content);		
		}
	}
	
}
