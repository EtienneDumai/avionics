# Compilateur
CXX = g++

# Flags
CXXFLAGS = -Wall -Wextra -Werror -std=c++20

# Nom de l'exécutable
TARGET = avionics

# Fichiers sources
SRCS = main.cpp Simulator.cpp AircraftState.cpp Display.cpp

# Fichiers objets (remplace .cpp par .o)
OBJS = $(SRCS:.cpp=.o)

# Règle principale
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compilation des .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(TARGET)