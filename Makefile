# Compilateur
CXX = g++

# Flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++20

# Nom de l'exécutable
TARGET = avionics

# Linker pour les libs
# SDL2/SDL2_ttf sont liées en STATIQUE (pas besoin d'installer
# libsdl2/libsdl2-ttf sur la machine qui exécute le binaire final).
# Leurs dépendances système (X11, freetype, ALSA/Pulse, Wayland...)
# restent en DYNAMIQUE : elles sont déjà présentes sur tout bureau
# Linux, et le chargement des drivers graphiques/audio a besoin du
# dynamic linker pour fonctionner correctement.
STATIC_LIBS  = -lSDL2 -lSDL2_ttf
DYNAMIC_LIBS = $(filter-out $(STATIC_LIBS),$(shell pkg-config --static --libs sdl2 SDL2_ttf))
LDLIBS = -Wl,-Bstatic $(STATIC_LIBS) -Wl,-Bdynamic $(DYNAMIC_LIBS)
# Fichiers sources (exclut src/test/ : ce sont les sources des tests
# GoogleTest, compilées séparément par CMake, pas par ce Makefile)
SRCS = $(shell find src -type f -name "*.cpp" -not -path "src/test/*")

# Fichiers objets (remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# Règle principale
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

# Compilation des .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

print:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
