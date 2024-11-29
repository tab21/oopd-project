all : debug opt

debug :
	g++ -g wifi4.cpp -o d4.o
	g++ -g wifi5.cpp -o d5.o
	g++ -g wifi6.cpp -o d6.o

opt :
	g++ -o3 wifi4.cpp -o o4.o
	g++ -o3 wifi5.cpp -o o5.o
	g++ -o3 wifi6.cpp -o o6.o

clean :
	rm -rf d4.o d5.o d6.o o4.o o5.o o6.o