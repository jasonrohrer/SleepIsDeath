

# 
# Generic:
#
# Map all .cpp C++ and C files into .o object files
#
# $@   represents the name.o file
# $<   represents the name.cpp file
#
%.eps: %.png
	convert $< $@
%.eps: %.sk
	sk2ps $< $@
%.dvi: %.tex
	latex $<
%.ps: %.dvi
	dvips -Ppdf -t letter -o $@ $<
%.pdf: %.ps
	ps2pdf $<


all: controllerGuide.pdf

clean: 
	rm *.pdf *.ps *.dvi *.log *.aux


controllerGuide.dvi: screen.eps close.eps stack.eps search.eps replace.eps edit.eps clear.eps