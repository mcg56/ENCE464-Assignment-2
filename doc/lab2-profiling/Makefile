all: profiling.pdf

%.pdf: %.tex
	pdflatex -shell-escape $*
	pdflatex -shell-escape $*

clean:
	-rm *.pdf *.aux *.log *.out
	-rm -r _minted-profiling
