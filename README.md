# Práctica 2: itinerarios y tarifas del Metro

**Lenguajes y Paradigmas de la Computación — Universidad EAFIT**

**Integrantes:** Miguel Salazar y Juan Sebastián.

## ¿Qué hace el programa?

El programa calcula cuánto paga un pasajero al hacer un recorrido en varios transportes del Metro. Tiene en cuenta el tipo de transporte, el perfil del pasajero y los transbordos que puede hacer gratis. Al final muestra el recorrido ordenado y un resumen de los cobros.

Partimos del `main.cpp` del profesor y completamos las partes pendientes. Dejamos sus ejemplos y agregamos comentarios cortos en español para identificar las partes del código. Los nombres de las clases y los mensajes de salida se mantienen como los pide la práctica.

## Archivos

- `main.cpp`: programa completo con comentarios por partes.
- `README.md`: explicación del trabajo e instrucciones para ejecutarlo.
- `Defensa_Practica_2.docx`: guía de apoyo para preparar la presentación entre los dos integrantes.

## Cómo ejecutarlo

Se necesita un compilador compatible con C++17. Desde la carpeta donde está `main.cpp`, se compila con:

```bash
g++ -std=c++17 -Wall -Wextra -o practice main.cpp
```

En Linux o macOS:

```bash
./practice
```

En Windows, desde PowerShell:

```powershell
.\practice.exe
```

También se puede pegar el archivo en OnlineGDB y seleccionar C++17. No hay que escribir datos: el `main()` del profesor ya contiene los transportes, los pasajeros y los viajes de prueba.

## Cómo está organizado

1. **Transportes:** `TransportMode` reúne los datos y métodos comunes. Sus clases hijas definen las tarifas y las reglas de cada transporte.
2. **Pasajeros:** `Passenger` calcula el cobro según el perfil. `fareOn()` evita que el resultado sea negativo.
3. **Red:** `Network` guarda los transportes usando `unique_ptr`. Los viajes solo guardan una referencia mediante un puntero al transporte que utilizan.
4. **Recorrido:** `chargeItinerary()` ordena los viajes por minuto y aplica la ventana de 90 minutos, con un máximo de dos transbordos gratis.
5. **Reportes:** las funciones usan algoritmos STL y lambdas para sumar, contar, agrupar y filtrar.
6. **Demostración:** el `main()` crea los ejemplos y muestra los resultados.

Los comentarios separan la red y el recorrido para facilitar la explicación. Los reportes corresponden a la parte 4 del enunciado, aunque el comentario del código los numera como parte 5.

## Resultados del ejemplo

| Resultado | Valor |
| --- | ---: |
| Total del pasajero frecuente | $20.640 |
| Transbordos gratuitos | 2 |
| Total del estudiante | $17.800 |
| Total del adulto mayor | $19.170 |
| Primer viaje con cobro | Metro A |

El frecuente paga el Metro A, el Metro B y el cable turístico. El cable K y el tranvía son los dos transbordos gratis. El alimentador cuesta cero para ese perfil, pero no cuenta como transbordo ni abre la ventana.

El estudiante paga $1.600 en el alimentador porque este admite la tarifa plana estudiantil. El cable turístico no admite esa tarifa ni la integración. Estas son las reglas del ejercicio; el recargo turístico es un valor didáctico del enunciado.

## Preguntas para la defensa

### ¿Por qué el destructor de TransportMode es virtual?

La red guarda los transportes en `unique_ptr<TransportMode>`, aunque los objetos sean de clases hijas. El destructor virtual permite destruir correctamente el objeto hijo al eliminarlo mediante el puntero a la clase base. Sin él, esa eliminación tiene comportamiento indefinido.

### ¿Por qué usamos una clase abstracta y polimorfismo en vez de un enum con switch?

Porque cada transporte puede responder cuál es su tarifa y qué reglas permite. El cálculo del recorrido usa esos métodos sin preguntar qué clase concreta recibió. Por ejemplo, el cable turístico responde que no admite integración. Así se puede agregar otra clase de transporte sin añadir casos por tipo al cálculo del cobro.

La primera pregunta está identificada en el PDF. Como no aparece una segunda pregunta numerada, incluimos la comparación de diseño que plantea la introducción.

## Comprobaciones y reglas de diseño

La versión ejecutable se verificó en C++17 con `-Wall -Wextra -Werror -pedantic`, sin errores ni advertencias. Los resultados coinciden con los del PDF. Después solo se añadieron comentarios de apoyo.

No usamos `dynamic_cast`, `typeid` ni comparaciones de nombres para decidir tarifas. Los reportes no tienen ciclos explícitos y el cable turístico obtiene la tarifa de `CableLine::baseFare()`. El único ciclo que añadimos está en `chargeItinerary()`; el de `formatCOP()` ya venía en el archivo del profesor.

## Material utilizado

- Enunciado y archivo base de la Práctica 2.
- Diapositivas de programación orientada a objetos del curso.
- Ejemplo del Metro de las diapositivas de Smalltalk, especialmente herencia, polimorfismo, llamadas al padre y bloques.
