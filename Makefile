SRC=$(shell ls ??-*.md ???-*.md)
DST=$(SRC:%.md=%.html)
DST2=$(SRC:%.md=./pdf/%.pdf)
PDFILTERGRAPHVIZ=~/repos/pandoc-filter-graphviz/pandoc-filter-graphviz

.PHONY=all clean

all: ${DST2} 

html: ${DST2}

%.html: %.md
#	pandoc -t revealjs -s -o myslides.html slides.md  -V revealjs-url=http://lab.hakim.se/reveal-js
	pandoc -t revealjs -s -o $@ -V revealjs-url=reveal.js -V center=false --include-in-header custom.css $<

./pdf/%.pdf: %.md
	pandoc -F $(PDFILTERGRAPHVIZ) -t beamer -s -o $@ -V theme:Warsaw $<

clean:
	rm -f ${DST}
