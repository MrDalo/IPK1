# IPK 2022 1. project

Simple HTTP server which provide basic information about the system as CPU usage,
hostname and CPU name. This server is written in C language. 

## How to run

```
$make
$./hinfosvc [port_number]
```

port_number is namber of port, which will server listen to.


## Used libraries

* stdio.h
* stdlib.h
* stdbool.h
* string.h
* arpa/inet.h
* sys/socket.h
* unistd.h

## Usage


1. Run server in the first command line
2. In the second command line send request
3. Kill server with CTR+C
### Type of requests:
```
curl http://localhost:12345/hostname
```
- this request will show hostname of the server's system

```
curl http://localhost:12345/cpu-name
```
- this request will show CPU name of the server's machine

```
curl http://localhost:12345/load
```
- this request will show CPU usage

### Example
```
./hinfosvc 12345 
curl http://localhost:12345/hostname
curl http://localhost:12345/cpu-name
curl http://localhost:12345/load

```

## Author

### Dalibor Králik, xkrali20


