SHELL := /bin/bash
TARGET = cannon
ARGS = mat1.txt mat2.txt

ifndef n
	override n = 16
endif

default: pre-build cannon

pre-build: 
	@echo set context
	source /opt/nfs/config/source_bupc.sh

cannon: $(TARGET).c
	@echo start building
	upcc -gupc -network=udp -pthreads=4 ./$(TARGET).c -o $(TARGET)
	@echo done
	
run:
	/opt/nfs/config/station_name_list.sh 201 216 > nodes
	UPC_NODEFILE=nodes upcrun -c 4 -N 4 -n $(n) ./$(TARGET) $(ARGS) > wynik.txt

clean:
	-rm -f cannon
	-rm -f wynik.*
	-rm -f *.log
	-rm -f *.log?
	-rm -f Output.*
	-rm -f .nfs*

