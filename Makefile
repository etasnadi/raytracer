CC = g++

# Config nvcc
CUDACC = nvcc
CUDA_ARCH = compute_60
CUDA_ARGS = -Xcudafe --diag_suppress=3057

# Config Qt
MOC = moc
QT_INCLUDE_PATH = /usr/include/x86_64-linux-gnu/qt5
QT_INCLUDE_PATHS = -I$(QT_INCLUDE_PATH) -I$(QT_INCLUDE_PATH)/QtGui -I$(QT_INCLUDE_PATH)/QtWidgets -I$(QT_INCLUDE_PATH)/QtCore
QT_LIBS = -lQt5Gui -lQt5Widgets -lQt5Core

SRC_DIR= ../src
HEADERS_DIR= ../src

# Device objects compiled with nvcc
OBJS = renderer.o triangle.o sphere.o shader.o room_scene.o raytracer_basics.o models.o basic_types.o 
OBJS_CU = cutracer.o
DEVICE_CODE = device_code.o

# Qt objects: moc compilation and Qt link
QT_OBJS = display.o
QT_MOC_OBJS = $(addprefix moc_, $(QT_OBJS))
QT_MOC_SRCS = $(QT_MOC_OBJS:.o=.cpp)

# Main app Qt link
MAIN_OBJ = app.o
MAIN_EXEC = app

# Links everything (device fatbinary + qt objects + main app)
all: $(DEVICE_CODE) $(QT_OBJS) $(MAIN_OBJ)
	$(CC) $(MAIN_OBJ) $(OBJS_CU) $(QT_MOC_OBJS) $(QT_OBJS) $(DEVICE_CODE) $(OBJS) -lcudadevrt -lcudart -o $(MAIN_EXEC) $(QT_LIBS)

clean:
	rm $(OBJS) $(OBJS_CU) $(MAIN_OBJ) $(QT_MOC_OBJS) $(QT_MOC_SRCS) $(QT_OBJS) $(DEVICE_CODE) $(MAIN_EXEC)

# Compile device code
$(OBJS): %.o : $(SRC_DIR)/%.cc
	$(CUDACC) $(CUDA_ARGS) -x cu -arch=$(CUDA_ARCH) -I$(HEADERS_DIR) -dc $(SRC_DIR)/$< -o $@

$(OBJS_CU): %.o : $(SRC_DIR)/%.cu
	$(CUDACC) $(CUDA_ARGS) -x cu -arch=$(CUDA_ARCH) -I$(HEADERS_DIR) -dc $(SRC_DIR)/$< -o $@

$(DEVICE_CODE): $(OBJS) $(OBJS_CU)
	$(CUDACC) $(CUDA_ARGS) -arch=$(CUDA_ARCH) -dlink $(OBJS) $(OBJS_CU) -o $@

# Compile mocs
$(QT_MOC_SRCS): moc_%.cpp: $(SRC_DIR)/%.h
	$(MOC) $< > $@

$(QT_OBJS): %.o : $(SRC_DIR)/%.cc $(SRC_DIR)/%.h moc_%.cpp
	$(CC) -fPIC $(QT_INCLUDE_PATHS) $(QT_LIBS) -c moc_$*.cpp -o moc_$@
	$(CC) -fPIC $(QT_INCLUDE_PATHS) $(QT_LIBS) -c $(SRC_DIR)/$*.cc -o $@

# Compile main
$(MAIN_OBJ): %.o: $(SRC_DIR)/%.cc
	$(CC) -fPIC $(QT_INCLUDE_PATHS) $(QT_LIBS) -c $(SRC_DIR)/$*.cc -o $@