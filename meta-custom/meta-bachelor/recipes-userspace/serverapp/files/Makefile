TARGET = serverapp

serverapp_SOURCES = serverapp.cpp
serverapp_OBJECTS = $(serverapp_SOURCES:.cpp=.o)
serverapp_LDFLAGS = -lnl-genl-3 -lnl-3 -lpistache -lpthread -latomic
serverapp_CXXFLAGS = -Wall -pedantic -g -O1 -std=c++17

all: $(TARGET)

$(TARGET): $(serverapp_OBJECTS)
	$(CXX) $^ $(LDFLAGS) $(serverapp_LDFLAGS) -o $@

$(serverapp_OBJECTS): %.o: %.cpp
	$(CXX) $^ $(CXXFLAGS) $(serverapp_CXXFLAGS) -c -o $@

clean:
	rm -fr $(TARGET) $(serverapp_OBJECTS)


install: $(TARGET)
	mkdir -p $(DESTDIR)/$(BINDIR)
	install -m 0755 $< $(DESTDIR)/$(BINDIR)

