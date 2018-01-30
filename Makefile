## This whole Makefile could be the following line:
# emcc -Wall -Werror --bind -I. -O2 --memory-init-file 0 -o curvelib.html CurveBridge.cpp Curve.cpp CurveFunctions.cpp

TARGET = curvelib.html

OBJS = CurveBridge.o Curve.o CurveFunctions.o
SOURCES = $(subst .o,.cpp,$(OBJS))
DEPS = Curve.h CurveFunctions.h jsassert.h

CXX = emcc
CFLAGS=-Wall -Werror --bind -I. -O2 --memory-init-file 0
LDFLAGS=

%.o: %.cpp $(DEPS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)

depend: .depend

.depend: $(SOURCES)
	$(RM) ./.depend
	$(CXX) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >> ./.depend

clean:
	$(RM) $(OBJS)
	$(RM) .depend

-include .depend
