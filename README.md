# NoGo-Framework

Framework for NoGo and similar games (C++ 11)
Modified from [2048-Framework](https://github.com/moporgic/2048-Framework)

## Basic Usage

To make the sample program:
```bash
make # see makefile for details
```

To run the sample program:
```bash
./nogo # by default the program runs 1000 games
```

To specify the total games to run:
```bash
./nogo --total=1000
```

To display the statistics every 1 episode:
```bash
./nogo --total=1000 --block=1 --limit=1
```

To specify the total games to run, and seed the player:
```bash
./nogo --total=1000 --black="seed=12345" --white="seed=54321"
```

To save the statistics result to a file:
```bash
./nogo --save=stats.txt
```

To load and review the statistics result from a file:
```bash
./nogo --load=stats.txt
```

## Advanced Usage

To specify custom player arguments (need to be implemented by yourself):
```bash
./nogo --total=1000 --black="search=MCTS timeout=1000" --white="search=alpha-beta depth=3"

./nogo --total=10 --black="search=mc-rave timeout=1000000" --white="search=zero timeout=5"

./nogo --total=10 --black="search=mc-rave timeout=1000000 nodeinit=true" --white=""

./nogo --total=10 --black="search=mc-rave timeout=1000000 nodeinit=true" --white="search=mc-rave timeout=5"

./nogo --total=10 --black="search=mc-rave timeout=1000000" --white="search=mc-rave timeout=1000000 nodeinit=true"
```

To launch the GTP shell and specify program name for the GTP server:
```bash
P1B='../nogo --shell --name="MyNoGo-black" --black="search=MCTS timeout=1000 nodeinit=true"'
P1W='../nogo --shell --name="MyNoGo-white" --white="search=MCTS timeout=1000 nodeinit=true"'
```

To launch the GTP shell with custom player arguments:
```bash
./nogo --shell --black="search=MCTS simulation=1000" --white="search=alpha-beta depth=3"
```

## Author

Theory of Computer Games, [Computer Games and Intelligence (CGI) Lab](https://cgilab.nctu.edu.tw/), NYCU, Taiwan
