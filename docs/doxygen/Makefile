.PHONY: all

all: 
	doxygen doxy.conf

display:
	firefox html/html/index.html

clean: 
	rm -rf html *.txt

install:
	cd html/latex/; make; sudo cp -f refman.pdf $(DONNIE_PATH)/docs/doxygen.pdf
