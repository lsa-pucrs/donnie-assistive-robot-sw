FOLDER := .
SOURCE := main
FILENAME := report_2016

compile: clean
	pdflatex $(FOLDER)/$(SOURCE).tex 
	bibtex   $(FOLDER)/$(SOURCE).bib
	pdflatex $(FOLDER)/$(SOURCE).tex 
	pdflatex $(FOLDER)/$(SOURCE).tex

display:
	gnome-open $(SOURCE).pdf

organize:
	rm -f $(SOURCE).log
	rm -f $(SOURCE).aux
	rm -f $(SOURCE).out
	rm -f $(SOURCE).bbl
	rm -f $(SOURCE).blg

clean: organize
	rm -f $(SOURCE).pdf

install:
	sudo cp -f  $(SOURCE).pdf $(DONNIE_PATH)/docs/$(FILENAME).pdf
