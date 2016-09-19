#####################################################
# A small LaTeX Makefile
#####################################################

PREFIX=main

CHAPTERS=hardware testcases verif_env main

all: subdirs $(PREFIX).pdf

#####################################################

.PHONY: subdirs 

subdirs:
	@for i in $(CHAPTERS); do \
	   (cd $$i; $(MAKE)); done

#####################################################

$(PREFIX).aux:  $(PREFIX).tex
	pdflatex $(PREFIX).tex

$(PREFIX).pdf: $(PREFIX).aux
	pdflatex $(PREFIX).tex

#####################################################

simple:
	pdflatex $(PREFIX).tex

#####################################################

clean:
	@for i in $(CHAPTERS); do \
	  (cd $$i; $(MAKE) clean); done
	rm -f $(PREFIX).aux $(PREFIX).bbl $(PREFIX).blg\
		$(PREFIX).idx $(PREFIX).log $(PREFIX).out $(PREFIX).tcp\
		$(PREFIX).toc $(PREFIX).tps $(PREFIX).prf $(PREFIX).lbl
	rm -f $(PDFFILES) *.aux
