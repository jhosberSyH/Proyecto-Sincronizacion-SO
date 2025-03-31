# Proyecto de Sincronizacion entre Procesos - Sistemas Operativos
## Prof. Mirella Herrera

Integrantes     |   Cédula
David Brito     | 30.521.360
Andres Gubaira  | 31.006.669
Josned Aguirre  | 31.083.291
Josue Carrion   | 30.020.470
Jhosber Ynojosa | 31.116.618

# Descripción del problema

El problema está dado en la necesidad contar con una sincronización entre procesos y un elevado grado de concurrencia para garantizar una ejecución fluída.

Se requiere proteger la sección crítica, que consta del número de mostradores, cintas transportadoras y almacenes. Con el uso de herramientas de sincronización (semáforos) se busca asegurar la exclusión mutua y evitar la espera activa.

# Funcionamiento del programa

El programa es un simulador de un sistema de aeropuerto que implementa un control de equipaje eficiente y seguro. Cuenta con la capacidad de lidiar con distintos tipos de equipaje con distintas prioridades cada uno; descarga de cierto tipo de equipajes, reconociendo su clasificación y estado, así como también un registro de los almacenes temporales donde se guardan los equipajes para ser transferidos a los aviones.

# Etapas del programa

1. Mostrador: lugar donde los pasajeros entregan su equipaje.
2. Cintas: transportan el equipaje al almacén correspondiente según su destino.
3. Almacenes: áreas de almacenamiento temporal que corresponden al destino del equipaje, en espera a ser cargado por los aviones.
4. Aviones: se cargan los equipajes correspondientes al destino del avión en base a la capacidad máxima de estos.
5. Terminal: el equipaje se traslada a la cinta donde los pasajeros recogerán su equipaje después del aterrizaje.

# Utilidades

- semaforo.h:

Archivo que contiene la clase "semaforo", utilizada como herramienta para garantizar la exclusión mutua.

- Equipaje.h

Definición de la clase "equipaje", con sus variables y funciones. Cuenta con una función para traducir la prioridad del equipaje, lo que corresponde a cómo se debe tratar el equipaje.

- almacen.h

Archivo donde se definen las variables y funciones de la clase "almacen". Cuenta con funciones de carga y descarga de equipaje.

- avion.h

Archivo que define la clase "avion". Cuenta con variables para identificar el destino del vuelo, así como el estado del avión (si hace falta descargar, o si está vacío). Contiene funciones para la carga, descarga y verificación del equipaje.

- interfaz.h

Archivo con la clase "interfaz", que es la encargada de incrementar la abstracción ofreciendo una interfaz visual para que el usuario pueda entender con mayor facilidad los resultados. Cuenta con dos modos de operación distintos:

1. Modo Supervisor: permite visualizar el estado del CPU, el estado de la memoria y cantidad de hilos usados en tiempo real.

2. Modo Usuario: incluye funciones para ver las distintas etapas del programa, así como una forma de verlas todas. Además, incluye una función para buscar 3 equipajes específicos y rastrearlos a través de las etapas.

# Parámetros

Se definieron constantes a la hora de realizar el programa que pueden ser modificadas a gusto del programador. Estas se conforman por:

```c
// Cantidades de hilos
#define MAX_MOSTRADORES 5000    //Cantidad de mostradores disponible
#define MAX_CINTAS 500          //Cantidad de cintas de transporte
#define MAX_ALMACEN 250         //Cantidad de almacenes temporales
#define MAX_TERMINALES 200      //Cantidad de terminales disponible
```