cxx = g++
cxxflags = -Wall -pedantic -std=c++11

ifeq ($(OS), Windows_NT)
	# Windows
else 
	UNAME_S := $(shell uname -s)
 #    ifeq ($(UNAME_S),Linux)
	# 	cxxflags += -D LINUX
 #    endif
    ifeq ($(UNAME_S),Darwin)

		objcc = g++
		objcflags = -lobjc -framework Cocoa
		objcsrc = awcocoa/cocoa.mm

    endif
 #    UNAME_P := $(shell uname -p)
 #    ifeq ($(UNAME_P),x86_64)
	# 	cxxflags += -D AMD64
 #    endif
 #    ifneq ($(filter %86,$(UNAME_P)),)
	# 	cxxflags += -D IA32
 #    endif
 #    ifneq ($(filter arm%,$(UNAME_P)),)
	# 	cxxflags += -D ARM
 #    endif
endif


cxxsrc = test.cpp


all: test

clean:
	@rm ./a.out

kernel:
	# $(cxx) $()
	$(objcc) $(objcflags) $(objcsrc)

test:
	$(cxx) $(cxxflags) $(objcflags) $(objcsrc) $(cxxsrc)

wrap:
	$(cxx) $(cxxflags) $(cxxsrc)

cocoa:
	$(cxx) $(cxxflags) $(objcflags) $(objcsrc) $(cxxsrc)



