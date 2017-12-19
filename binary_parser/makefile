#Compiler and linker
CXX 		:= g++

#Directories and File Extensions
SRCDIR		:= ./src
BUILDDIR  	:= ./build
TARGETDIR 	:= ./
INCDIR 		:= ./inc
SRCEXT 		:= cpp
INCEXT    	:= h
OBJEXT 		:= exe
DEPEXT		:= d

TARGET 		:= test.$(OBJEXT)

#Includes, Flags
DEBUG	 		:= -g
STD 			:= -std=c++17
CXXFLAGS		:= -Wall $(STD) $(DEBUG)
INC				:= -I$(INCDIR) -I/usr/include/gtest #add other direcectories here 
LIB				:= -L/usr/lib/gtest -lgtest -lgtest_main -lpthread  

#for debugging and variables
print-%: 
	@echo $*=$($*)

#finds *.h in incdir
HEADERS 	:= $(shell find $(INCDIR) -type f -name *.$(INCEXT)) 
SOURCES 	:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))
	
all: directories $(TARGET)

directories:
				@mkdir -p $(INCDIR)
				@mkdir -p $(SRCDIR)
				@mkdir -p $(BUILDDIR)
				@echo made directories

-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

$(TARGET): $(OBJECTS)
			@echo headers $(HEADERS)
			@echo sources $(SOURCES)
			@echo objects $(OBJECTS)
			$(CXX) -o $@ $^ $(LIB)

$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
				$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<
				$(CXX) $(CXXFLAGS) $(INC) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
				@echo  compiling

remake: cleaner all
clean: 
			@$(RM) -rf $(BUILDDIR)
			@echo cleaning objects only

cleaner: clean
				@$(RM) -rf $(TARGETDIR)/$(TARGET)
				@echo cleaning objects libraries binaries

install:
			   mkdir -p /usr/local/include/zinhart/
			   mkdir -p /usr/local/include/zinhart/inc
			   cp ./inc/csv_parser.h /usr/local/include/zinhart/inc
			   cp ./inc/byte_packer.h /usr/local/include/zinhart/inc
			   cp csv_parser /usr/local/include/zinhart/ 
			   cp byte_packer /usr/local/include/zinhart/
removeinstall:
				rm -rf /usr/local/include/zinhart/inc/csv_parser.h
				rm -rf /usr/local/include/zinhart/inc/byte_packer.h
				rm -rf /usr/local/include/zinhart/csv_parser
				rm -rf /usr/local/include/zinhart/byte_packer

.PHONY: all remake clean cleaner 

