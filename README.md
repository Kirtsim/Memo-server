## Memo server
C++ implementation of a server for the Memo application.

#### Pre-requirements:
- Docker

#### Build steps
```
$ git clone https://github.com/Kirtsim/Memo-server
$ cd Memo-server
$ docker pull kirtsim/memo-server:dev
$ docker run -it --privileged --rm -w /home/memo-server -v `pwd`:/home/memo-server kirtsim/memo-server:dev ./setup
$ docker run -it --privileged --rm -w /home/memo-server -v `pwd`:/home/memo-server kirtsim/memo-server:dev ./compile
```

#### Running the server:
```
$ ./build/memo_server 127.0.0.1 8000 "_"
```

#### Shut down:
- hit CTRL+C
