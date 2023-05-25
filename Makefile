include config.mk

all: waitd


HDR = util.hpp args.hpp
SRC = waitd.cpp util.cpp args.cpp
OBJ = $(SRC:.cpp=.o)


.cpp.o:
	$(CXX) $(CFLAGS) $(CPPFLAGS) -c $<

$(OBJ): $(HDR)

waitd: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o $@


clean:
	rm -f $(OBJ) waitd

install: waitd
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install ./waitd $(DESTDIR)$(PREFIX)/bin/waitd

uninstall:
	rm $(PREFIX)/bin/waitd

.PHONY: install uninstall clean
