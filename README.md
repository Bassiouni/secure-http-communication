# Encrypted Messaging System
Simple Encrypted Messaging System Over HTTP Protocol

## How to use

compile code first
```sh
g++ -O3 -std=c++20 main.cc src/*.cc -o client -Iinclude
```

Then run the executable file providing the `host` and `port`

First client
```sh
./client localhost 3000
```

Second client
```sh
./client localhost 3001
```
