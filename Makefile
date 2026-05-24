# Compilateur
CXX = g++

# Flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++20

# Nom de l'exécutable
TARGET = avionics

# Linker pour les libs
LDLIBS = -lSDL2 -lSDL2_ttf
# Fichiers sources
SRCS = $(shell find src -type f -name "*.cpp")

# Fichiers objets (remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# Règle principale
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDLIBS) -o $(TARGET) $(OBJS)

# Compilation des .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(TARGET)

print: 
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
