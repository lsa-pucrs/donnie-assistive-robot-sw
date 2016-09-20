FOLDER := .
SOURCE := main
FILENAME := user_manual

compile: clean
	pdflatex $(FOLDER)/$(SOURCE).tex 
	#bibtex $(SOURCE).aux
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
	rm -f $(SOURCE).toc

clean: organize
	rm -f $(SOURCE).pdf

install:
	sudo cp -f  $(SOURCE).pdf $(DONNIE_PATH)/docs/$(FILENAME).pdf
