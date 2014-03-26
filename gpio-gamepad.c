#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev-user.h>

#define KEY_PRESSED				1
#define KEY_RELEASED			0

#define TRUE 					0
#define FALSE					1
#define KEY_UP                  103
#define KEY_LEFT                105
#define KEY_RIGHT               106
#define KEY_DOWN                108

#define KEY_A                   30
#define KEY_S                   31
#define KEY_D                   32
#define KEY_F                   33

#define KEY_Z                   44
#define KEY_X                   45

#define KEY_Q                   16
#define KEY_W                   17

#define AltKeyBit               17

struct GPIO_KEY {
	char bitPosition;

	char keyCode;
	char isNormalModeEnabled;

	char keyAltCode;
	char isAltModeEnabled;
	char AltDelay;
};


struct input_event     	ev;
int                 	fd;

int sendKeyEvent(char keycode,char status) {
	//printf("Keycode %x  Status %x\n",keycode,status);
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_KEY;
	ev.code = keycode;
	ev.value = status;
	return write(fd, &ev, sizeof(struct input_event));
}

int sendSync() {
	memset(&ev, 0, sizeof(struct input_event));
	ev.type = EV_SYN;
	ev.code = SYN_REPORT;
	ev.value = 0;
	return write(fd, &ev, sizeof(struct input_event));
}




int main () {

	struct uinput_user_dev 	uidev;
	
	int						gpiosValue;
	struct GPIO_KEY 		keys[12];
	char 					isAltPressed;
	int x;
	FILE *pf;
	int temp;
	int sync;
	char buf[2];
	int file;
	char keyDown;
	short int test;
	char *filename = "/dev/i2c-2";




	keys[0].keyCode = KEY_UP;
	keys[0].isNormalModeEnabled = FALSE;
	keys[0].bitPosition = 7;
	keys[0].keyAltCode = -1;
	keys[0].isAltModeEnabled = FALSE;
	keys[0].AltDelay = -1;


	keys[1].keyCode = KEY_DOWN;
	keys[1].isNormalModeEnabled = FALSE;
	keys[1].bitPosition = 5;
	keys[1].keyAltCode = -1;
	keys[1].isAltModeEnabled = FALSE;
	keys[1].AltDelay = -1;

	keys[2].keyCode = KEY_LEFT;
	keys[2].isNormalModeEnabled = FALSE;
	keys[2].bitPosition = 4;
	keys[2].keyAltCode = -1;
	keys[2].isAltModeEnabled = FALSE;
	keys[2].AltDelay = -1;

	keys[3].keyCode = KEY_RIGHT;
	keys[3].isNormalModeEnabled = FALSE;
	keys[3].bitPosition = 6;
	keys[3].keyAltCode = -1;
	keys[3].isAltModeEnabled = FALSE;
	keys[3].AltDelay = -1;

	keys[4].keyCode = KEY_Z; //Start
	keys[4].isNormalModeEnabled = FALSE;
	keys[4].bitPosition = 2;
	keys[4].keyAltCode = -1;
	keys[4].isAltModeEnabled = FALSE;
	keys[4].AltDelay = -1;

	keys[5].keyCode = KEY_X; //Select
	keys[5].isNormalModeEnabled = FALSE;
	keys[5].bitPosition = 3;
	keys[5].keyAltCode = -1;
	keys[5].isAltModeEnabled = FALSE;
	keys[5].AltDelay = -1;

	keys[6].keyCode = KEY_Q; // L
	keys[6].isNormalModeEnabled = FALSE;
	keys[6].bitPosition = 13;
	keys[6].keyAltCode = -1;
	keys[6].isAltModeEnabled = FALSE;
	keys[6].AltDelay = -1;

	keys[7].keyCode = KEY_W; //R 
	keys[7].isNormalModeEnabled = FALSE;
	keys[7].bitPosition = 12;
	keys[7].keyAltCode = -1;
	keys[7].isAltModeEnabled = FALSE;
	keys[7].AltDelay = -1;

	keys[8].keyCode = KEY_A; //Y
	keys[8].isNormalModeEnabled = FALSE;
	keys[8].bitPosition = 11;
	keys[8].keyAltCode = -1;
	keys[8].isAltModeEnabled = FALSE;
	keys[8].AltDelay = -1;

	keys[9].keyCode = KEY_S; // X
	keys[9].isNormalModeEnabled = FALSE;
	keys[9].bitPosition = 8;
	keys[9].keyAltCode = -1;
	keys[9].isAltModeEnabled = FALSE;
	keys[9].AltDelay = -1;


	keys[10].keyCode = KEY_D; // B
	keys[10].isNormalModeEnabled = FALSE;
	keys[10].bitPosition = 10;
	keys[10].keyAltCode = -1;
	keys[10].isAltModeEnabled = FALSE;
	keys[10].AltDelay = -1;

	keys[11].keyCode = KEY_F; // A
	keys[11].isNormalModeEnabled = FALSE;
	keys[11].bitPosition = 9;
	keys[11].keyAltCode = -1;
	keys[11].isAltModeEnabled = FALSE;
	keys[11].AltDelay = -1;

	if ((file = open(filename, O_RDWR)) < 0) {
	    /* ERROR HANDLING: you can check errno to see what went wrong */
	    perror("Failed to open the i2c bus");
	    exit(1);
	}

	int addr = 0x20;          // The I2C address of the ADC
	if (ioctl(file, I2C_SLAVE, addr) < 0) {
	    printf("Failed to acquire bus access and/or talk to slave.\n");
	    /* ERROR HANDLING; you can check errno to see what went wrong */
	    exit(1);
	}

	// Enable Pullups on Bank A and Bank B
	if (i2c_smbus_write_word_data (file,0x0C, 0xFFFF) != 0) {
		printf("Error");
	}



	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if(fd < 0)
		return 1;

	if(ioctl(fd, UI_SET_EVBIT, EV_KEY) < 0)
		return 1;

	if(ioctl(fd, UI_SET_EVBIT,EV_SYN) < 0)
		return 1;


	for(x = 0; x < 12 ; x++) {
		if ( ioctl(fd, UI_SET_KEYBIT,keys[x].keyCode) < 0)
			return 1;

		if( keys[x].keyAltCode != -1 ) {
			if ( ioctl(fd, UI_SET_KEYBIT,keys[x].keyAltCode) < 0)
				return 1;
		}

	}

	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor  = 0x1;
	uidev.id.product = 0x1;
	uidev.id.version = 1;

	if(write(fd, &uidev, sizeof(uidev)) < 0)
		return 1;

	if(ioctl(fd, UI_DEV_CREATE) < 0)
		return 1;

	printf("uinput entry created and running");
	while (1) {
		sync = 0;
		isAltPressed = FALSE;

		gpiosValue =  i2c_smbus_read_word_data(file,0x12);

		/*	Internal pull up resistors used.
			I2C expander will return the following for each gpio:
				Button Up = 1
				Button Down = 0

			Flip bits since it makes more sense when the opposite is true 
		*/

		

		gpiosValue = ~gpiosValue & 0xFFFF;



		if ( gpiosValue & (1 << AltKeyBit)) {
			isAltPressed = TRUE;
		}

		for(x = 0; x < 12; x++) {

			keyDown = ((gpiosValue & ( 1 << keys[x].bitPosition)) > 0 ) ? TRUE : FALSE;

			//printf("Keydown: %x\n",keyDown);

			if(keyDown == FALSE) {
				if(keys[x].isAltModeEnabled == TRUE) {
					sendKeyEvent(keys[x].keyAltCode,KEY_RELEASED);
					//printf("Alt mode");
					sync = 1;
				}

				if(keys[x].isNormalModeEnabled == TRUE) {
					sendKeyEvent(keys[x].keyCode,KEY_RELEASED);
					//printf("Keycode %x Released\n",keys[x].keyCode);
					sync = 1;
				}

				keys[x].isNormalModeEnabled = FALSE;
				keys[x].isAltModeEnabled = FALSE;

			} else {
				if (keys[x].isAltModeEnabled == TRUE && isAltPressed == FALSE) {
					sendKeyEvent(keys[x].keyAltCode,KEY_RELEASED);
					sendKeyEvent(keys[x].keyCode,KEY_PRESSED);

					sync = 1;

					keys[x].isNormalModeEnabled = TRUE;
					keys[x].isAltModeEnabled = FALSE;


				} else if (keys[x].isAltModeEnabled == FALSE && isAltPressed == TRUE && keys[x].keyAltCode != -1) {
					sendKeyEvent(keys[x].keyAltCode,KEY_PRESSED);
					sendKeyEvent(keys[x].keyCode,KEY_RELEASED);

					sync = 1;


					keys[x].isNormalModeEnabled = FALSE;
					keys[x].isAltModeEnabled = TRUE;
				} else if (keys[x].isNormalModeEnabled == FALSE) {
					//printf("Keycode %x Pressed\n",keys[x].keyCode);
					sendKeyEvent(keys[x].keyCode,KEY_PRESSED);

					sync = 1;

					keys[x].isNormalModeEnabled = TRUE;
				}

			}
		
		}

		if(sync == 1)
			sendSync();

		usleep(16670);

	}


cleanup_ioctl:

    if(ioctl(fd, UI_DEV_DESTROY) < 0)
    	die("error: ioctl");

cleanup_uinput:
    close(fd);

cleanup_i2chandle:
	close(file);
}

