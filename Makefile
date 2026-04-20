CXX      = clang++
CC       = clang
CXXFLAGS = -std=c++17 -O3 -Wno-deprecated-declarations
CFLAGS   = -O3

TARGET   = game_engine_linux

INCLUDES = -I. \
		   -I./src/third_party \
		   -I./src/third_party/glm \
		   -I./src/third_party/rapidjson \
		   -I./SDL2 \
		   -I./SDL_image \
		   -I./SDL_mixer \
		   -I./SDL_ttf \
		   -I./src/third_party/lua \
		   -I./src/third_party/LuaBridge \

LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -ldl

CPP_SOURCES = src/first_party/*.cpp
C_SOURCES = src/third_party/lua/*.c

game:
	$(CC) $(CFLAGS) -c $(C_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CPP_SOURCES) *.o $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)