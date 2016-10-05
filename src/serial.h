#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <stdarg.h>


// Data structures
int tty_fd;


// API Implementation

/** Initiate a serial connection to the given tty */
void serial_init(const char* tty) {
	struct termios tio;
	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag=ICANON;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;

	tty_fd = open(tty, O_RDWR | O_NOCTTY);
	cfsetospeed(&tio,B115200);            // 115200 baud
	cfsetispeed(&tio,B115200);            // 115200 baud
	tcsetattr(tty_fd,TCSANOW,&tio);
}

/** Send a command through the serial link */
void serial_command(const char * command, ... ) {
	char cmd[512];
	va_list args;
	va_start (args, command);
	vsprintf (cmd, command, args);
	va_end (args);
	strcpy(&cmd[strlen(cmd)], "\r");
	write(tty_fd, cmd, strlen(cmd));
}

/** Close the serial link */
void serial_close() {
	close(tty_fd);
}


void create_fifo(const char* path) {
	char cmd[1024];
	sprintf(cmd, "sudo mkfifo %s 2>/dev/null; sudo chmod a+wr %s", path, path);
	system(cmd);
}
