# marcel-mobile

A char-device broker to communicate with Marcel's MCU

marcel-mobile creates several UNIX FIFOs, and mux/demux various corresponding channels to the MCU's tty :
* /dev/motors : _output_ To send speed commands to the left and right motors 
* /dev/roam : _output, binary_ To switch between roaming/joystick modes
* /dev/us : _input_, Receive the US sensors data
* /dev/odo : _input_, Receive the odometric integration (x,y,theta)


Data is simply issued in text, and fields are space-separated, one record per line

[https://raw.githubusercontent.com/roule-marcel/stuff/master/doc/archi.png](archi)
