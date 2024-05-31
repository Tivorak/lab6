# Path to vitis header files
INC=/home/dirk/.programs/vivado/Vitis_HLS/2022.2/include

OBJDIR=build
UTILDIR=../util
SRCDIR=src

USRCS=$(wildcard $(UTILDIR)/*.cpp)
OBJS=$(patsubst $(UTILDIR)/%.cpp,$(OBJDIR)/%.o,$(USRCS))

SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS += $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))




FLAGS += -I$(UTILDIR) -I$(SRCDIR) -I${INC} -g -O3 -fsanitize=address


TRG=sobel
LFLAGS+=-pthread

$(TRG): $(OBJS)
	$(CXX) $(LFLAGS) $(FLAGS)  $^ -o $@ 


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp  $(SRCDIR)/sobel.h
	$(CXX) $(FLAGS) -c $< -o $@ 


$(OBJDIR)/%.o : $(UTILDIR)/%.cpp $(UTILDIR)/%.h
	$(CXX) $(FLAGS) -c $< -o $@





clean:
	$(RM) build/*
	$(RM) $(TRG)







