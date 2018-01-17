#####################################################
# A small LaTeX Makefile
#####################################################

CHAPTERS=doxygen manual/hardware_manual manual/software_manual manual/user_manual reports/2016

all: subdirs

#####################################################

.PHONY: subdirs 

subdirs:
	@for i in $(CHAPTERS); do \
	   (cd $$i; $(MAKE)); done

#####################################################
#####################################################

clean:
	@for i in $(CHAPTERS); do \
	  (cd $$i; $(MAKE) clean); done

install:
	@for i in $(CHAPTERS); do \
	  (cd $$i; $(MAKE) install); done
