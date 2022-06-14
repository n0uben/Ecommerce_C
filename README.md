
# FlexCode

FlexCode is an app server / client replicating a simplified e-commerce website, using sockets and multithreading.


## Authors

- [@n0uben](https://www.github.com/n0uben)
- [@Shaxens](https://github.com/Shaxens)
- [@DrYruo](https://github.com/DrYruo)


## Features

- Connections allowed with sockets
- Multiple connections allowed with multithreading
- Variables protection with mutex & semaphores


## Run Locally

__Clone the project__

```bash
  git clone https://github.com/n0uben/FlexCode.git
```

__Go to the project directory__

```bash
  cd FlexCode
```

__Compile sources__

```bash
  gcc -lpthread client.c -o client
  gcc -lpthread serveur.c -o serveur
```

__Run server & client__ (open a terminal for each)

Terminal #1 (first)
```bash
  ./serveur
```
Terminal #2
```bash
  ./client
```

