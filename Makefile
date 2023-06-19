
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

EXTERNAL_LIBS = glew

#linkage
LIBS = -lglfw -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -lpthread -ldl `pkg-config --libs $(EXTERNAL_LIBS)` -lm -lOpenCL

# 14 is fine for now, might need to upgrade ?
CXXFLAGS = -std=gnu++14 -Wall -Wextra
CXXFLAGS += -O3 #better optimization
#CXXFLAGS += -fno-omit-frame-pointer #linux profiler
#CXXFLAGS += -Wno-deprecated-declarations # hide usleep deprecated warning

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ -O3 -o $@ $(LIBS)

obj/%.o:src/%.cpp includes/*.h src/*.hpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@ -Iincludes -Ilibft -I$(HOME)/.brew/include

clean :
	rm -rf obj

fclean : clean
	rm -rf $(NAME)

re : fclean all

