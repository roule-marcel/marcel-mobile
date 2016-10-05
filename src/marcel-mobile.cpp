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
				serial_command("%c %s", c, line);
			}
		}
	}
};

class ProxyIn {
public:
	thread th;
	char c;
	string path;
	int fd;
	ProxyIn(char c, const char* path) {
		fd = -1;
		this->c = c;
		this->path = path;

		th = thread([&]{run(); });
	}

	void run() {
		printf("Proxy '%c' -> '%s'\n", c, path.c_str());
		fd = publish_out(path.c_str());
		offer_transport(path.c_str(), "tcp://");
		if(fd==-1) {fprintf(stderr, "Error : can't open output channel %s\n", path.c_str()); exit(1); }
	}

	void cmd(const char* command) {
		if(fd!=-1) pubsub::send(fd, command);
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

void read_conf(const char* file) {
	FILE* fConf = fopen("marcel-mobile.conf", "r");
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
	}
}

void do_proxy(char c, const char* cmd) {
	ProxyIn* p = PROXIES_IN[c];
	if(!p)  fprintf(stderr, "Unknown command : %c %s\n", c, cmd);
	else p->cmd(cmd);
}

int main(int argc, char const *argv[]) {
	serial_init("/dev/ttyAMA0");
	read_conf("marcel-mobile.conf");

	FILE* fTTY = fdopen(tty_fd, "r");
	if(!fTTY) { fprintf(stderr, "Error : Can't talk to /dev/ttyAMA0\n"); exit(1); }
	for(;;) {
		char line[1024];
		while (fgets(line, 1024, fTTY) != NULL)  {
			if(line[strlen(line)-1]!='\n') line[strlen(line)-1] = '\n';
			char c = line[0];
			do_proxy(c, line[1] == ' ' ? &line[2] : &line[1]);
		}
	}

	return 0;
}
