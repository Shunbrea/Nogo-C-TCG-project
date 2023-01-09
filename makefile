all:
	g++ -std=c++11 -O3 -g -Wall -fmessage-length=0 -o nogo nogo.cpp -I ~/anaconda3/envs/py38/include/python3.8/ -lpython3.8
clean:
	rm nogo