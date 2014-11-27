CPPFLAGS += -I/usr/local/pccts-1.33/inpracticaude -I/usr/inpracticaude/pccts
CXXFLAGS += -Wno-write-strings

all: compiler

compiler: compiler.o ptype.o interpreter.o symtab.o scan.o err.o 
	$(CXX) -o $@ $^

compiler.c err.c parser.dlg tokens.h: compiler.g
	antlr -gt $<

mode.h scan.c: parser.dlg
	dlg -ci $< scan.c

compiler.o: compiler.c mode.h tokens.h myASTnode.hh
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

ptype.o interpreter.o symtab.o: %.o: %.cc %.hh myASTnode.hh
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

scan.o err.o: %.o: %.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


compilerclean:
	$(RM) compiler compiler.o err.o ptype.o scan.o interpreter.o \
		symtab.o util.o compiler.c err.c mode.h parser.dlg scan.c tokens.h

jps := $(sort $(wildcard jp?)) $(sort $(wildcard jp??))

diff: compiler
	@for i in $(jps); do \
		echo $$i; \
		./compiler execute < $$i > sortida; \
		diff s$$i sortida; \
	done
	@$(RM) sortida
