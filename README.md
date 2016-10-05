# marcel-mobile

A char-device broker to communicate with Marcel's MCU

marcel-mobile creates several UNIX FIFOs (for outgoing channels) and publish libpubsub channels (for incoming channels), and mux/demux corresponding data to the MCU's tty :
* /dev/motors : _output_ To send speed commands to the left and right motors (argv=[left,right,timeout])
* /dev/beep : _output_ Just beep (argv=[duration])
* /dev/roam : _output, binary_ To switch between roaming/joystick modes (argv=[0/1])
* /dev/us : _input_, Receive the US sensors data (fmt=[f,f,f,f,f])
* /dev/odo : _input_, Receive the odometric path integration (fmt=[x,y,theta])


Data is simply issued as text, and fields are space-separated, one record per line

![software-architecture](https://raw.githubusercontent.com/roule-marcel/stuff/master/doc/archi.png)
