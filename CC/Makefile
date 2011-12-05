.PHONY: all clean test

APPNAME = lucc.exe
SOURCES_PATH = source
OBJECTS_PATH = objects
SOURCES = $(notdir $(wildcard $(SOURCES_PATH)/*.cpp))
OBJECTS = $(addprefix $(OBJECTS_PATH)/, $(patsubst %.cpp, %.o, $(SOURCES)))
INCLUDES = -Iheaders

CXXFLAGS += -Wall -Wextra -Wno-unused -Wno-switch $(INCLUDES)

all: $(APPNAME)

test: all
	$(APPNAME) --test

clean:
	$(RM) $(APPNAME) $(OBJECTS)

$(OBJECTS): | $(OBJECTS_PATH)

$(OBJECTS_PATH):
	mkdir $@

$(OBJECTS_PATH)/%.o: $(SOURCES_PATH)/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDFLAGS)

$(APPNAME): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)