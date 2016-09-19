FOLDER := .
SOURCE := main
FILENAME := hardware_manual

compile: clean
	pdflatex $(FOLDER)/$(SOURCE).tex 
	#bibtex $(SOURCE).aux
	pdflatex $(FOLDER)/$(SOURCE).tex 
	pdflatex $(FOLDER)/$(SOURCE).tex
	rm -f ../../$(FILENAME).pdf
	cp $(SOURCE).pdf ../../$(FILENAME).pdf

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
