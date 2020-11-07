
NAME = particleSystem

FILES = main.cpp \
		Utilities.cpp \
		Shader.cpp \
		Loop.cpp \
		Camera.cpp \
		Matrix.cpp \
		Vec3.cpp \
		clProgram.cpp \
		Particles.cpp \
		appWindow.cpp

OBJ = $(addprefix obj/,$(FILES:.cpp=.o))

#linkage
LIBS = -lglfw -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -lpthread -ldl `pkg-config --libs glew` -lm -lOpenCL

UNAME = $(shell uname -s)
ifneq (, $(findstring MINGW, $(UNAME)))
	LIBS = -lglfw3 -lgdi32 -lglew32 -lopengl32
	NAME = humangl.exe
endif

# 14 is fine for now, might need to upgrade ?
CXXFLAGS = -std=gnu++14 -Wall -Wextra
CXXFLAGS += -g #GNU debugger
CXXFLAGS += -fno-omit-frame-pointer #linux profiler
CXXFLAGS += -Wno-deprecated-declarations # hide usleep deprecated warning

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@ $(LIBS)

obj/%.o:src/%.cpp includes/*.h $(wildacrd $(src/%.hpp))
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Iincludes -Ilibft

clean :
	rm -rf obj

fclean : clean
	rm -rf $(NAME)

re : fclean all

