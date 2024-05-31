OBJDIR=build
UTILDIR=../util
SRCDIR=src

USRCS=$(wildcard $(UTILDIR)/*.cpp)
OBJS=$(patsubst $(UTILDIR)/%.cpp,$(OBJDIR)/%.o,$(USRCS))

SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS += $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))



XILINX_XRT=/usr/include/xrt/

CXX_FLAGS += -g -std=c++17 -I${XILINX_XRT} -I${SRCDIR} -I${UTILDIR} -O3
LD_FLAGS = -lxrt_coreutil -pthread -lbload


TRG=sobel

$(TRG): $(OBJS)
	$(CXX) $(CXX_FLAGS)  $^ -o $@ $(LD_FLAGS) 


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp  
	$(CXX) $(CXX_FLAGS) -c $< -o $@ 


$(OBJDIR)/%.o : $(UTILDIR)/%.cpp $(UTILDIR)/%.h
	$(CXX) $(CXX_FLAGS) -c $< -o $@





clean:
	$(RM) build/*
	$(RM) $(TRG)
