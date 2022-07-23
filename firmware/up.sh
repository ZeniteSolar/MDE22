
#cppcheck  --force --std=c99 --language=c src/
make clean
rm -rf bin obj
make all
#fuser -k /dev/ttyACM0
avrdude -c arduino -P /dev/ttyS8 -p m328p -D -U flash:w:bin/firmware.elf
stty 57600 < /dev/ttyS8 && cat /dev/ttyS8  