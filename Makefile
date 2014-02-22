all: skplst

skplst: SkipList.cpp Main.cpp
	g++ SkipList.cpp Main.cpp -o skplst

clean:
	rm -f *.o skplst

