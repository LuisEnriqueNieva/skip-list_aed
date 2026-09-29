# Animación de la Skip List

La animación está impulsada por la implementación real de `../skip_list.cpp`:

1. `generar_trazas.cpp` incluye `skip_list.cpp` **sin modificarlo** (renombra su `main`) y ejecuta
   cada escena con `verbose = true`. La `SkipList` imprime una traza: `AVANZA`, `PARA`, `MONEDA`,
   `ENLAZA`, `DESENLAZA`, `SUBE_TECHO`, `BAJA_TECHO`, ...
2. `motor.py` lee esa traza y convierte cada línea en una animación de Manim. No decide nada por su
   cuenta: el puntero se mueve solo cuando la traza dice `AVANZA`, las flechas cambian solo con `ENLAZA`
   o `DESENLAZA`, etc.
3. Al final de cada escena, `ListaVisual.verificar()` compara lo dibujado con la foto `ESTADO` que
   imprime el C++. Si no coinciden, el render falla.
4. El panel de código muestra las funciones copiadas en vivo desde `skip_list.cpp`.

## Requisitos

- `g++` con C++17 en el PATH (en Windows sirve el MinGW de CLion:
  `C:\Program Files\JetBrains\CLion 2025.2\bin\mingw\bin`).
- Python 3.12 y Manim Community (probado con 0.21).

## Reproducir el video

```bash
cd animacion
python -m venv .venv
.venv\Scripts\pip install -r requirements.txt
.venv\Scripts\python -m manim -qh -a escenas.py
```

En Mac/Linux: `.venv/bin/pip` y `.venv/bin/python`. Manim necesita Python 3.9–3.13 (3.14 todavía puede
fallar al instalar). El programa C++ se compila solo la primera vez que se renderiza una escena.

- Borrador rápido de una escena: `.venv\Scripts\python -m manim -ql escenas.py A2_Busqueda`
- Videos: `media/videos/escenas/1080p60/*.mp4`
- Trazas usadas (evidencia para el informe): `trazas/*.txt`
- Ritmo: `VEL=0.8` hace las animaciones de la traza 25 % más lentas (PowerShell: `$env:VEL="0.8"`).

## Escenas

| Escena | Contenido | Traza |
|---|---|---|
| `A0_Portada`, `A0_Concepto` | qué es, TDA, usos | — |
| `A1_ListaVsSkip` | lista común (9 avances) → se agregan los niveles reales | `lineal`, `busqueda` |
| `A2_Busqueda` | `buscar(45)` y `buscar(33)` con el código de `descender` | `busqueda` |
| `B1_Insercion` | `insertar(15)` (altura 1) e `insertar(7)` (altura 4, sube el techo) | `insercion` |
| `B2_Eliminacion` | `eliminar(12)` y `eliminar(7)` (el techo baja de 4 a 2) | `eliminacion` |
| `C1_CasosBorde` | lista vacía, un elemento, duplicado, inexistente, peor caso | `bordes`, `lineal` |
| `C2_Complejidad` | avances promedio medidos para N = 16 … 262 144 | `experimento` |
| `C3_Cierre` | cómo se hizo + conclusiones | `busqueda` |

## Semillas

- Búsqueda y casos borde: semilla 42 (la misma del `main` de `skip_list.cpp`).
- Inserción y eliminación: semilla 1489, elegida para que se vea subir y bajar el techo.
  La eliminación arranca exactamente con la lista que dejó la inserción.
- `rand()` no da la misma secuencia en todos los compiladores (MinGW/MSVC vs. glibc). Las trazas del
  video se generaron con el MinGW de CLion en Windows; en otra plataforma las alturas pueden cambiar.
