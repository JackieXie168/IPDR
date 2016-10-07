# *****************************************************
# * File                 : makefile                   *
# * Description          : Main IPDRLIBS makefile     *
# *                        calls all other makefiles  *
# * Author               : Infosys Tech Ltd           *
# * Modification History :                            *
# *---------------------------------------------------*
# * Date       Name             Change/Description    *
# *---------------------------------------------------*
# * 01/28/02  Infosys           Created               *         
# *****************************************************


TARGETS=FTP Test

all : $(TARGETS)

FTP:
	cd ./source/ftp ; make

Test:
	cd ./test ; make all

clean:
	cd ./source/ftp ; make clean
	cd ./test ; make clean

# End of the main makefile
