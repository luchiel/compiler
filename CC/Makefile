.PHONY: all clean test

APPNAME = lucc.exe
SOURCES_PATH = source
OBJECTS_PATH = objects
SOURCES = $(notdir $(wildcard $(SOURCES_PATH)/*.cpp))
OBJECTS = $(addprefix $(OBJECTS_PATH)/, $(SOURCES:.cpp=.o))

export CPATH = $(CURDIR)/headers
CXXFLAGS += -Wall -Wextra -Wno-unused -Wno-switch

all: $(APPNAME)

test: all
	$(APPNAME) --test

clean:
	$(RM) $(APPNAME) $(OBJECTS) $(DEPENDENCIES)

$(OBJECTS): | $(OBJECTS_PATH)

$(OBJECTS_PATH):
	mkdir $@

DEPENDENCIES = $(OBJECTS:.o=.d)
-include $(DEPENDENCIES)

$(OBJECTS_PATH)/%.o: $(SOURCES_PATH)/%.cpp
	$(CXX) -c -o $@ -MMD $(CXXFLAGS) $<

$(APPNAME): $(OBJECTS)
	$(CXX) -o $@ $^