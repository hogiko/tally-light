# Project config

PROJECTS_HOME = $(HOME)/projects/conxult.de/arduino

ARDUINO_DIR=/opt/arduino
ARDMK_DIR=/usr/share/arduino

AVR_TOOLS_DIR=/usr/include


AVRDUDE      = /usr/bin/avrdude
AVRDUDE_CONF = /etc/avrdude.conf

ARDUINO_LIB_PATH = $(PROJECTS_HOME)/libraries/
ARDUINO_LIBS = NeoPixelBus_by_Makuna 

MONITOR_PORT = /dev/ttyUSB0
MONITOR_CMD = screen-wrap

#BOARD_TAG = pro
#BOARD_SUB = 16MHzatmega328
BOARD_SUB = ESP8266_NODEMCU_ESP12E

# "Platform" config
ARDUINO_QUIET = true
ARDUINO_SKETCHBOOK = $(PROJECTS_HOME)/TallyLight

#ARDMK_DIR = $(HOME)/Build/Arduino-Makefile
 
# Include parent Makefile from <https://github.com/sudar/Arduino-Makefile>
include /usr/share/arduino/Arduino.mk
