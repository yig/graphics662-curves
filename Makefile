TARGET = index.html

CXX = emcc

CFLAGS = -Wall -Werror --bind
OBJS = CurveBridge.o Curve.o CurveFunctions.o
SOURCES = $(subst .o,.cpp,$(SOURCES))
DEPS = Curve.h CurveFunctions.h jsassert.h

%.o: %.c $(DEPS)
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
