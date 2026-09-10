# Compilateur
CXX = g++

# Flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++20

# Nom de l'exécutable
TARGET = avionics

# Fichiers sources (exclut src/test/ : ce sont les sources des tests
# GoogleTest, compilées séparément par CMake, pas par ce Makefile)
SRCS = $(shell find src -type f -name "*.cpp" -not -path "src/test/*")

# Fichiers objets (remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# ------------------------------------------------------------------
# Build par défaut : lien DYNAMIQUE classique. Fonctionne partout
# tant que libsdl2/libsdl2-ttf sont installées (dev quotidien).
# ------------------------------------------------------------------
LDLIBS = -lSDL2 -lSDL2_ttf

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

# Compilation des .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ------------------------------------------------------------------
# Build de release : SDL2/SDL2_ttf en STATIQUE (binaire distribuable
# sans dépendance à libsdl2/libsdl2-ttf côté utilisateur final).
# Nécessite libSDL2.a / libSDL2_ttf.a -- présentes sur Ubuntu
# (libsdl2-dev/libsdl2-ttf-dev), PAS fournies par le paquet Arch
# officiel. À utiliser uniquement en CI/release, pas en dev local sur
# une distro qui ne fournit pas les .a.
# ------------------------------------------------------------------
STATIC_LIBS  = -lSDL2 -lSDL2_ttf
DYNAMIC_LIBS = $(filter-out $(STATIC_LIBS),$(shell pkg-config --static --libs sdl2 SDL2_ttf))

.PHONY: release
release: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -Wl,-Bstatic $(STATIC_LIBS) -Wl,-Bdynamic $(DYNAMIC_LIBS)

# Nettoyage
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: print
print:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
