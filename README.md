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

#### Before starting the server
```
$ docker network create --subnet 172.20.0.0/16 --ip-range 172.20.240.0/20 memo_network
```
You will need 3 terminal windows to run and test the server. One for the server itself, second for the client and third for stopping the server/client.

#### Starting the server:
\[Terminal 1\] In the cloned Memo-server directory:
```
$ docker run --rm --privileged -it \
    -w /home/memo-server \
    -v `pwd`:/home/memo-server \
    --net memo_network \
    --name server \
    --ip 172.20.20.20 \
    kirtsim/memo-server:dev \
    bash -c "./build/memo_server 172.20.20.20 8000"
```

#### Running the client:
\[Terminal 2\] In the cloned Memo-server directory:
```
$ docker run --rm --privileged -it \
    -w /home/memo-server \
    -v `pwd`:/home/memo-server \
    --name client \
    --net memo_network \
    kirtsim/memo-server:dev \
    bash -c "./build/memo_client 172.20.20.20 8000"
```

#### Shutdown server or client:
\[Terminal 3\]
```
$ docker stop server
$ docker stop client
```
