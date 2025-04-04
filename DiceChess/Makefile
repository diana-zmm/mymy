# Variablen
CXX = g++                     
CXXFLAGS = -std=c++17 -Wall    
OBJDIR = obj                  
BINDIR = bin                   

chess: main.o chess.o dice.o
	$(CXX) main.o chess.o dice.o -o chess

main.o: main.cpp 
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

chess.o: chess.cpp chess.h
	$(CXX) $(CXXFLAGS) -c chess.cpp -o chess.o

dice.o: dice.cpp dice.h
	$(CXX) $(CXXFLAGS) -c dice.cpp -o dice.o


# Aufräumen der erstellten Dateien
clean:
	rm main.o chess.o dice.o chess

