# David: El Camino a la Victoria

Proyecto Final - Programacion Grafica (DIS414)

Videojuego 2D basado en la historia biblica de David y Goliat, desarrollado con Raylib 5.0 y C++.

## Requisitos previos

- [MSYS2](https://www.msys2.org/) instalado en `C:\msys64`
- Abrir **UCRT64** desde el menu de inicio de MSYS2

Dentro de la terminal UCRT64, instalar los paquetes necesarios:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-raylib
```

## Compilacion

Clonar el repositorio y entrar a la carpeta:

```bash
git clone https://github.com/Oni12/DavidAndGoliat.git
cd DavidAndGoliat
```

Compilar con un solo comando:

```bash
g++ src/main.cpp src/game.cpp src/player.cpp src/level1.cpp src/level2.cpp src/level3.cpp src/tiles.cpp src/utils.cpp -o "David_Goliat.exe" -I"C:/msys64/ucrt64/include" -L"C:/msys64/ucrt64/lib" -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++17
```

O ejecutar directamente el archivo `build.bat` incluido en el repositorio:

```bash
./build.bat
```

## Ejecutar

```bash
./David_Goliat.exe
```

## Niveles

### Nivel 1 - El Arroyo
Recolecta 5 piedras esquivando a los soldados filisteos. Una vez reunidas, enfrenta a Goliat con la honda.

### Nivel 2 - El Valle de Ela
Derrota a Goliat esquivando sus jabalinas. Dispara tus 5 piedras con precision.

### Mapa Mundi
Pantalla de transicion entre niveles con puntos de interes interactivos (Gat, Cueva de Adulam) que muestran informacion historica y arqueologica con enlaces a YouTube.

### Nivel 3 - La Fortaleza de Gat
Enfrenta a un gigante filisteo en combate cuerpo a cuerpo con espada. El gigante ataca con un arma de mayor alcance y dispara proyectiles.

## Controles

- **WASD / Flechas**: Movimiento
- **ESPACIO**: Atacar / Disparar
- **ENTER**: Continuar / Confirmar
- **Mouse**: Apuntar (Nivel 1 y 2), Clic en puntos de interes (Mapa)

## Creditos

Musica:
- Chivalry 2 (Original Game Soundtrack)
- God of War 2018 Soundtrack
- Age of Empires 2

Assets graficos generados proceduralmente con Raylib.
