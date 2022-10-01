# SOURCES := $(shell find . -name "*.c")
SOURCES := Player.c
SOURCES2 := Recorder.c

R: $(SOURCES2)
	gcc $(SOURCES2) -o recorder -lasound

P: $(SOURCES)
	gcc $(SOURCES) -o player -lasound

List: ListPcm.c
	gcc ListPcm.c -o list -lasound

D: devices.c
	gcc devices.c -o devices -lasound