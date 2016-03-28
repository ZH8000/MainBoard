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


void uartReceiverCallback(UartInterface * uartInterface, char * content) {
	int whichBoard = -1;
	DaughterBoard * daughterBoard = getDaughterBoard(uartInterface, &whichBoard);
	
	if (daughterBoard != NULL) {
		//debugMessage("Receive DB[%d]: %s\n", whichBoard, content);
		int commandLength = strlen(content);
		bool isPingCommand = 
			content[0] == '$' && 
			content[1] == 'P' &&
			content[2] == 'I' &&
			content[3] == 'N' &&
			content[4] == 'G' &&
			content[5] == '$' &&
			(content[6] == '0' || content[6] == '1') &&
			content[7] == '$' &&
			(content[8] == '0' || content[8] == '1') &&
			content[commandLength-1] == '$';
		
		bool isResponse = content[0] == '#' && content[2] == '#' && content[commandLength-1] == '#';
		if (isPingCommand) {
			int whichTB = content[6] - 48;
			bool isInserted = content[8]  == '1';
			namedInterface.daughterBoards[whichBoard].isBoardInserted[whichTB] = isInserted;
			if (commandLength == 47) {
				strncpy(namedInterface.daughterBoards[whichBoard].uuid[whichTB], content + 10, 36);
				debugMessage("UUID2:%s\n", namedInterface.daughterBoards[whichBoard].uuid[whichTB]);
			}
			debugMessage("PING Command: %s, whichTB = %d, isInserted = %d, len=%d\n", content, whichTB, isInserted, strlen(content));
		} else if (isResponse) {
			sendToUART(&uartInterfaces[0], "Received daughterBoard[%d]: %s\n", whichBoard, content);
		}
	} else if (uartInterface == namedInterface.pcUART) {
		debugMessage("Received from PC: %s\n", content);		
	}
	
}
