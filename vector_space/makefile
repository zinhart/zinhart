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
			   mkdir -p /usr/local/include/zinhart/src
			   mkdir -p /usr/local/include/zinhart/inc
			   cp ./config /usr/local/include/zinhart/
			   cp ./declarations /usr/local/include/zinhart/	
			   cp ./zinhart_traits /usr/local/include/zinhart/
			   cp ./sequence_container /usr/local/include/zinhart/
			   cp ./steady_timer /usr/local/include/zinhart/
			   cp ./iterator /usr/local/include/zinhart/
			   cp ./vector_space /usr/local/include/zinhart/
			   cp ./utility /usr/local/include/zinhart/
			   
			   cp ./inc/config.h /usr/local/include/zinhart/inc
			   cp ./inc/declarations.h /usr/local/include/zinhart/inc
			   cp ./inc/zinhart_traits.h /usr/local/include/zinhart/inc
			   cp ./inc/sequence_container.h /usr/local/include/zinhart/inc		  
			   cp ./inc/steady_timer.h /usr/local/include/zinhart/inc		  
			   cp ./inc/iterator.h /usr/local/include/zinhart/inc
			   cp ./inc/vector_space.h /usr/local/include/zinhart/inc
			   cp ./inc/utility.h /usr/local/include/zinhart/inc
			   
			   cp -r ./src/*.tpp /usr/local/include/zinhart/src
removeinstall:
			
			   rm -rf /usr/local/include/zinhart/config
			   rm -rf /usr/local/include/zinhart/declarations
			   rm -rf /usr/local/include/zinhart/zinhart_traits
			   rm -rf /usr/local/include/zinhart/sequence_container
			   rm -rf /usr/local/include/zinhart/steady_timer
			   rm -rf /usr/local/include/zinhart/iterator
			   rm -rf /usr/local/include/zinhart/vector_space
			   rm -rf /usr/local/include/zinhart/utility
			   
			   rm -rf /usr/local/include/zinhart/inc/config.h
			   rm -rf /usr/local/include/zinhart/inc/declarations.h
			   rm -rf /usr/local/include/zinhart/inc/zinhart_traits.h
			   rm -rf /usr/local/include/zinhart/inc/sequence_container.h		  
			   rm -rf /usr/local/include/zinhart/inc/steady_timer.h		  
			   rm -rf /usr/local/include/zinhart/inc/iterator.h
			   rm -rf /usr/local/include/zinhart/inc/vector_space.h
			   rm -rf /usr/local/include/zinhart/inc/utility.h
			   rm -rf /usr/local/include/zinhart/src/*.tpp 

.PHONY: all remake clean cleaner 

