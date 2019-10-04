all:
	g++ -std=c++0x main1.cpp -o main1
	g++ -std=c++0x main2.cpp -o main2

clean:
	rm main1
	rm main2
