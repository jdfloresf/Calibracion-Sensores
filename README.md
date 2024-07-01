# Calibración y Lectura de Sensores de la Serie MQ

Este repositorio contiene tres códigos de Arduino diseñados para la calibración y lectura de sensores de la serie MQ. Los programas incluidos son:

1. `calibracion`: Obtiene el valor de R0.
2. `ratio`: Calcula la relación Rs/R0.
3. `final`: Muestra las lecturas de los sensores en PPM.

## Descripción

### Calibración

El programa `calibracion.ino` está diseñado para obtener el valor de R0 del sensor MQ. Este valor es esencial para calcular la concentración de gases en el aire.

### Cálculo de la Relación Rs/R0

El programa `ratio.ino` calcula la relación entre Rs (resistencia del sensor en presencia de gas) y R0 (resistencia del sensor en aire limpio). Esta relación es utilizada para determinar la concentración de gases.

### Lectura Final en PPM

El programa `final.ino` muestra las lecturas de los sensores en partes por millón (PPM), que es una medida de la concentración de gases.

## Más información

- [Métodos de Calibración de Sensores de Gas de la Familia MQ parte 1](https://www.youtube.com/watch?v=oH9JUsK9KjM)
- [Métodos de Calibración de Sensores de Gas de la Familia MQ parte 2](https://www.youtube.com/watch?v=zvM-xAsbDFk)
- [SENSOR GAS MQ - CALCULO DE PPM - CALIBRACION](https://www.youtube.com/watch?v=ihxuKCD-zFw)
