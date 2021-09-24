TITLE="CS323 Operating Systems"
AUTHOR="Mathias Payer"
DATE="EPFL, Fall 2021"

#SRC=$(shell ls ??-*.md ???-*.md)
SRC=$(shell ls ??-*.md)
DSThtml=$(SRC:%.md=%.html)
DSTpdf=$(SRC:%.md=./pdf/%.pdf)

.PHONY=all pdf clean

all: ${DSTpdf}

pdf: ${DSTpdf}

./pdf/%.pdf: %.md
	pandoc -f markdown+emoji -t beamer -s -o $@ -V theme:Warsaw \
		--metadata=title:${TITLE} --metadata=author:${AUTHOR} --metadata=date:${DATE} -H preamble.tex $<

clean:
	rm -f ${DST}
