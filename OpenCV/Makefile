#-----Macros---------------------------------                                                                                                                                                         
# set up compiler options                                                                                                                                                                              
OPENCV_INCLUDE_DIR=/afs/ir/class/cs232/opencv-2.4.5/install/include
OPENCV_LIBRARY_DIR=/afs/ir/class/cs232/opencv-2.4.5/install/lib
CXX = g++
INCLUDES = -I. -I$(OPENCV_INCLUDE_DIR)
CXXFLAGS = $(INCLUDES) -O3 -g

# set up linker options                                                                                                                                                                               
LFLAGS = -L$(OPENCV_LIBRARY_DIR) \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_features2d \
        -lopencv_calib3d \
        -lopencv_ml \
        -lopencv_video \
        -lopencv_objdetect

#-----File Dependencies----------------------                                                                                                                                                         
EXE = test

SRC = main.cc

OBJ = $(addsuffix .o, $(basename $(SRC)))

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

#-----Other stuff----------------------------                                                                                                                                                          
depend:
	makedepend $(CXXFLAGS) -Y $(SRC)

clean:
	rm -f $(OBJ) $(EXE) *.cc~ *.h~ Makefile~