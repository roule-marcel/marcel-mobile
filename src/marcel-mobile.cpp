#include "serial.h"
#include <string>
#include <map>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <libpubsub.h>

using namespace std;
using namespace pubsub;


class ProxyOut {
public:
	thread th;
	string path;
	char c;
	ProxyOut(char c, const char* path) {
		this->c = c;
		this->path = path;

		th = thread([&]{ run(); });
	}

	void run() {
		printf("Proxy '%s' -> '%c'\n", path.c_str(), c);
		create_fifo(path.c_str());
		while(1) {
			FILE* f = fopen(path.c_str(), "r");
			if(!f) { fprintf(stderr, "Error : can't open input file %s\n", path.c_str()); exit(1); }
			char line[1024];
			while (fgets(line, 1024, f) != NULL)  {
				if(line[strlen(line)-1]=='\n') line[strlen(line)-1] = 0;
				if(c) serial_command("%c %s", c, line);
				else serial_command("%s", line);
			}
		}
	}
};

class ProxyIn {
public:
	thread th;
	char c;
	string path;

	Channel* publisher;


	ProxyIn(char c, const char* path) {
		publisher = NULL;
		this->c = c;
		this->path = path;

		th = thread([&]{run(); });
	}

	void run() {
		printf("Proxy '%c' -> '%s'\n", c, path.c_str());
		publisher = publish(path);
	}

	void cmd(const char* command) {
		if(publisher) publisher->write(command);
	}
};

map<char,ProxyIn*> PROXIES_IN;
map<char,ProxyOut*> PROXIES_OUT;


void proxy_out(char c, const char* path) {
	PROXIES_OUT[c] = new ProxyOut(c, path);
}

void proxy_in(char c, const char* path) {
	PROXIES_IN[c] = new ProxyIn(c, path);
}

bool read_conf(const char* file) {
	FILE* fConf = fopen(file, "r");
	if(fConf) {
		char line[1024];
		while (fgets(line, 1024, fConf) != NULL)  {
			if(*line == '#') continue;
			if(line[strlen(line)-1]=='\n') line[strlen(line)-1] = 0;
			char* type = line;
			char* c = strstr(line, " ")+1; while(isspace(*c)) c++;
			char* path = strstr(c, " "); while(isspace(*path)) path++;
			if(strstr(type, "proxy_in") == type) proxy_in(*c, path);
			else proxy_out(*c, path);
		}
		fclose(fConf);
	}
	return fConf != NULL;
}

int main(int argc, char const *argv[]) {
	serial_init("/dev/ttyAMA0");
	FILE* fTTY = fdopen(tty_fd, "r");
	if(!fTTY) { fprintf(stderr, "Error : /dev/ttyAMA0 doesn't exist - Are you really on a Marcel robot ??? \n"); exit(1); }

	if(!read_conf("./marcel-mobile.conf")) {
		read_conf("/etc/marcel/marcel-mobile.conf");
	}

	PROXIES_OUT[0] = new ProxyOut(0, "/dev/robot");


	for(;;) {
		char line[1024];
		while (fgets(line, 1024, fTTY) != NULL)  {
			if(line[strlen(line)-1]!='\n') line[strlen(line)-1] = '\n';
			char c = line[0];
			char* cmd = (line[1] == ' ' ? &line[2] : &line[1]);
			ProxyIn* p = PROXIES_IN[c];
			if(!p) { fprintf(stderr, "Unknown command : %c %s\n", c, cmd); }
			else p->cmd(cmd);
		}
	}


	return 0;
}
