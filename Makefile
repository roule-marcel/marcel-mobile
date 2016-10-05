all: marcel-mobile

marcel-mobile: bin/marcel-mobile.o /usr/lib/libpubsub.so
	g++ -g -pthread -o -std=c++11 -o $@ $< -lpubsub

bin/%.o: src/%.cpp src/serial.h
	mkdir -p `dirname $@`
	g++ -g -pthread -std=c++11 -o $@ -c $<

clean:
	rm -rf bin
	rm -f marcel-mobile


install:
	sudo systemctl stop marcel-mobile.service
	sudo mkdir -p /usr/lib/marcel-mobile
	sudo cp -r marcel-mobile src /usr/lib/marcel-mobile
	sudo cp marcel-mobile.service /etc/systemd/system/
	sudo chmod a+x  /usr/lib/marcel-mobile/marcel-mobile
	sudo chmod a+rx /etc/systemd/system/marcel-mobile.service
	sudo chown root /etc/systemd/system/marcel-mobile.service
	sudo systemctl daemon-reload
	sudo systemctl restart marcel-mobile.service
	sudo systemctl enable marcel-mobile.service
