obj-m += lancha.o
all:
	make -C /lib/modules/6.7.5-100.fc38.x86_64/build M=$(PWD) modules 
	cp lancha.ko ./build 
clean:
	make -C /lib/modules/6.7.5-100.fc38.x86_64/build M=$(PWD) clean 
