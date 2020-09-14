## Memo server
C++ implementation of a server for the Memo application.

#### Pre-requirements:
- Docker

#### Build steps
```
$ git clone https://github.com/Kirtsim/Memo-server
$ cd Memo-server
$ docker pull kirtsim/memo-server:dev
$ ./run cmake build
```

#### Before starting the server
```
$ docker network create --subnet 172.20.0.0/16 --ip-range 172.20.240.0/20 memo_network
```

#### Starting the server:
```
$ ./run server
```

#### Shutdown running server
Open a new terminal window and execute:
```
$ docker stop server
```
