TARGET = curvelib.html

OBJS = CurveBridge.o Curve.o CurveFunctions.o
SOURCES = $(subst .o,.cpp,$(OBJS))
DEPS = Curve.h CurveFunctions.h jsassert.h

CXX = emcc
CFLAGS=-Wall -Werror --bind -I.
LDFLAGS=--bind

%.o: %.cpp $(DEPS)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

depend: .depend

.depend: $(SOURCES)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ >> ./.depend

clean:
	$(RM) $(OBJS)
	$(RM) .depend

-include .depend
