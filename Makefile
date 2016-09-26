all: 


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
