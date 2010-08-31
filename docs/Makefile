# create mobilec.pdf

all:
	sh ./split_text_files.sh
	pdflatex mobilec.tex
	bibtex mobilec
	makeindex mobilec.idx 
	pdflatex mobilec.tex
	pdflatex mobilec.tex

clean:
	rm -rf *.ps *.log *.dvi *.aux *.*% *.lof *.lop *.lot *.toc *.idx *.ilg *.ind *.bbl *.blg *.out
