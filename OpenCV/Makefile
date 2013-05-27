#-----Macros---------------------------------
INCLUDES = -I./include/
OPTFLAGS = -g -O2

# set up compiler options
CXX = g++
CXXFLAGS = $(INCLUDES) $(OPTFLAGS)

# setup relevant libraries
OPENCV_CFLAGS = `pkg-config --cflags opencv`

CXXFLAGS += $(OPENCV_CFLAGS) 

OPENCV_LFLAGS = `pkg-config --libs opencv`

LFLAGS += $(OPENCV_LFLAGS) -L../lib -ltessocr -ltesslib -llept 

# build configurations
OBJ_PATH 	= objs
SRC_PATH	= src
EXE			= ./extract_text

SRC = $(wildcard ./$(SRC_PATH)/*.cpp)
OBJ = $(addprefix $(OBJ_PATH)/, $(notdir $(addsuffix .o, $(basename $(SRC)))))

#-----Compile Rules---------------------------
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
	@echo -n "$(OBJ_PATH)/" > $@.d.tmp
	@$(CXX) $(CXXFLAGS) -MM $< >> $@.d.tmp
	@mv $@.d.tmp $@.d

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) -o $(EXE) $(OBJ) $(LFLAGS)

clean: 
	rm -f $(addsuffix .d, $(OBJ)) $(addsuffix .d.tmp, $(OBJ)) $(OBJ) $(EXE) 

#-----File Dependencies----------------------
-include $(addsuffix .d, $(OBJ))


