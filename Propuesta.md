<img width="1024" height="559" alt="image" src="https://github.com/user-attachments/assets/7354d3bd-3040-48af-ba8d-469d4e212efb" /># Ascensor Inteligente

**Autores:** Uma Toscan, Mariana Dos Reis, Felipe Colodro  
**Padrones:** 111106, 111545, 106433
**Fecha:** 2do cuatrimestre 2026  

---

## 1. Selección del proyecto a implementar

### 1.1 Objetivo del proyecto y resultados esperados
El objetivo de este proyecto es diseñar e implementar un sistema embebido de control para un ascensor inteligente, capaz de gestionar múltiples solicitudes de usuarios, optimizar recorridos, garantizar seguridad y registrar estadísticas de funcionamiento.

**Resultados esperados:**
* Control confiable del motor y las puertas.
* Interfaz de usuario con botoneras y pantalla LCD.
* Registro de eventos (pisos solicitados, fallas, tiempos de espera).
* Seguridad mediante sensores de puerta y sobrecarga.
* Posibilidad de expansión futura (conectividad, estadísticas avanzadas).

### 1.2 Proyectos similares
Se consideran tres alternativas:
1. **Ascensor básico:** Control de motor y botones físicos.
2. **Ascensor con LCD:** Incorpora pantalla LCD y registro de eventos.
3. **Ascensor avanzado:** Incluye conectividad y memoria persistente.

Para comparar estas alternativas se tienen en cuenta seis aspectos característicos:
1. **Disponibilidad de hardware (Peso: 9):** Facilidad de conseguir motores, sensores y placas.
2. **Seguridad (Peso: 7):** Robustez frente a fallas y protección de usuarios.
3. **Experiencia de usuario (Peso: 8):** Comodidad, tiempos de espera, interfaz clara.
4. **Tiempo de implementación (Peso: 8):** Factibilidad dentro del cuatrimestre.
5. **Costo (Peso: 6):** Balance entre calidad y presupuesto.
6. **Interés personal (8):** Motivación del equipo en trabajar con STM32 y periféricos.

| Proyecto | Puntaje Total |
| :--- | :---: |
| Ascensor básico | 344 |
| Ascensor con LCD | 358 |
| Ascensor avanzado | 268 |

*Tabla 1.2.1: Puntaje de proyectos*

#### Análisis detallado de los criterios:
* **Disponibilidad de hardware (9):** Los componentes principales —motores, sensores de puerta y sobrecarga, displays LCD y placas de control como STM32— se consiguen fácilmente en el mercado local. Esto asegura que cualquier versión del ascensor pueda implementarse sin depender de importaciones costosas o difíciles de obtener.
* **Facilidad de uso (8):** La comodidad del usuario es clave en un ascensor. Una interfaz clara con botoneras bien distribuidas y, en versiones más completas, una pantalla LCD que muestre el piso actual y mensajes de estado, mejora la experiencia y reduce errores.
* **Seguridad (7):** Todo ascensor debe garantizar protección básica: sensores de puerta, sobrecarga y parada de emergencia. Estos mecanismos aseguran un funcionamiento seguro frente a fallas comunes, aunque no eliminan riesgos externos como mal uso o problemas estructurales del edificio.
* **Tiempo de implementación (8):** El proyecto debe ser realizable dentro del cuatrimestre. Una arquitectura modular y el uso de máquinas de estados permiten organizar el desarrollo y cumplir plazos, tanto en versiones básicas como avanzadas.
* **Costo (6):** El ascensor requiere inversión en motor, sensores y pantalla. Aunque el gasto es moderado, se mantiene dentro de un rango razonable para un prototipo académico funcional. Se prioriza calidad y aprendizaje por sobre una minimización extrema del presupuesto.
* **Interés personal (8):** El proyecto es motivador porque combina control de motores, seguridad, interfaz hombre-máquina y almacenamiento de datos. Permite aplicar conocimientos de sistemas embebidos en un contexto real y desafiante, lo que lo convierte en una experiencia enriquecerora para el equipo.

#### Matriz de ponderación detallada:

| Criterio (Peso) | Ascensor básico (Puntaje) | Ascensor básico (Pond.) | Ascensor con LCD (Puntaje) | Ascensor con LCD (Pond.) | Ascensor avanzado (Puntaje) | Ascensor avanzado (Pond.) |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Disponibilidad de hardware (9)** | 8 | 72 | 9 | 81 | 4 | 36 |
| **Facilidad de uso (8)** | 6 | 48 | 8 | 64 | 5 | 40 |
| **Seguridad (7)** | 6 | 42 | 7 | 49 | 10 | 70 |
| **Tiempo de implementación (8)** | 9 | 72 | 8 | 64 | 5 | 40 |
| **Costo (6)** | 9 | 54 | 6 | 36 | 3 | 18 |
| **Interés personal (8)** | 7 | 56 | 8 | 64 | 8 | 64 |
| **Puntaje Total** | **-** | **344** | **-** | **358** | **-** | **268** |

*Tabla 1.2.2: Comparación de proyectos*

### 1.3 Selección de proyecto
Considerando la Tabla 1.2.1, se elige implementar el **ascensor con LCD**. El ascensor básico, si bien es más económico y rápido de implementar, no ofrece la misma experiencia de usuario ni las funcionalidades adicionales que permiten una interacción más clara con el sistema. Por otro lado, el ascensor avanzado con conectividad requiere mayor tiempo de desarrollo, integración de múltiples tecnologías y un presupuesto elevado, lo cual excede el margen de tiempo y recursos propuestos para este proyecto. Además, la conectividad introduce desafíos adicionales en términos de seguridad de datos y mantenimiento, que no resultan prioritarios en esta etapa.

La opción del ascensor con LCD se presenta como un equilibrio entre facilidad de uso, seguridad y costo. El display permite brindar información clara al usuario, como el piso actual, mensajes de estado o alertas, mejorando la experiencia sin comprometer la simplicidad del sistema. Asimismo, mantiene un nivel de seguridad adecuado y un tiempo de implementación razonable.

Este proyecto busca una alternativa moderna a los ascensores tradicionales, incorporando un módulo LCD que mejora la interacción con el usuario y aporta valor agregado frente a sistemas más básicos. Los desafíos principales de este proyecto son la integración del LCD con el controlador del ascensor, la optimización energética para asegurar un consumo eficiente, la preservación de la seguridad en el funcionamiento del sistema y la correcta gestión de los recursos de hardware disponibles.

#### Diagrama en bloques
En la Figura 1.3.1 (representada conceptualmente en la Tabla 1.3.1) se muestra el diagrama en bloques del sistema con los principales módulos del proyecto. El microcontrolador se ubica en el centro y coordina la interacción entre los distintos periféricos y sensores del ascensor.

* **Interfaz de usuario (GPIO):**
  * Botonera interior (cabina).
  * Botonera exterior (pisos).
  * Pantalla LCD para mostrar piso actual y mensajes de estado.
* **Sensores de seguridad (GPIO):**
  * Sensor de puerta (detecta apertura/cierre).
  * Sensor de sobrecarga (peso máximo permitido).
  * Botón de emergencia.
* **Control de movimiento (PWM):**
  * Driver de motor (regula la potencia y dirección del motor según las órdenes del microcontrolador).
  * Motor principal del ascensor (ejecuta el movimiento vertical entre pisos).
<img width="2816" height="1536" alt="diagrama_bloques png" src="https://github.com/user-attachments/assets/9c4e5405-73cf-48bd-98bb-526ab9e38c8a" />

---

## 2. Elicitación de requisitos y casos de uso

En Argentina existen varias empresas que ofrecen sistemas de ascensores con distintos niveles de automatización y seguridad. Aunque estos productos están orientados a uso comercial y residencial, ninguno coincide exactamente con el alcance del proyecto que estamos desarrollando, que busca una solución funcional y simplificada.

Un primer competidor es **Ascensores Cóndor**, que fabrica y mantiene ascensores electromecánicos en Buenos Aires. Sus sistemas incluyen botoneras digitales y control de puertas, pero no integran pantallas LCD para mostrar información detallada al usuario. Nuestro proyecto se diferencia al incorporar un display que mejora la interacción y la claridad de uso.
Link: http://www.ascensorescondor.com/?utm_source=ig&utm_medium=social&utm_content=link_in_bio&fbclid=PAZXh0bgNhZW0CMTEAc3J0YwZhcHBfaWQPOTM2NjE5NzQzMzkyNDU5AAGn13lDuAVi5dhtuCId50eIghyUosBH8x_WjyTWoXNa4tBrylbSO9jZsuktf2E_aem_K109Uy7iHPtJeuCJ0ARY3A
      https://www.instagram.com/ascensores_condor/?hl=es
      
Un segundo competidor es **Servas Ascensores**, que ofrece ascensores con sistemas de seguridad redundantes y control electrónico. Aunque robustos, sus soluciones están pensadas para edificios completos y requieren infraestructura más compleja. El proyecto propuesto busca replicar algunas funciones básicas de seguridad (sensores de puerta y sobrecarga) en una versión más acotada y accesible.
Link: https://landings.servas.com/?gad_source=1&gad_campaignid=23556090406&gbraid=0AAAABAiMmx_r5K5Qgpvc2ypYbnrdzSi9z&gclid=CjwKCAjw9szSBhBNEiwAC57Sq1DYON-RB880T5N8EgbA_3afA47-h3za1OiGujXkOTeRapGGny0F2xoCv-YQAvD_BwE

En resumen, el mercado argentino ofrece soluciones profesionales, pero el *Ascensor Inteligente* se posiciona como una alternativa realista y simplificada, que integra seguridad, control de movimiento y comunicación visual sin requerir infraestructura compleja.

### 2.1 Requisitos del proyecto

| Grupo | ID | Descripción |
| :--- | :---: | :--- |
| **Control de movimiento** | 1.1 | El sistema controlará el motor del ascensor mediante señales PWM. |
| | 1.2 | El sistema permitirá el desplazamiento entre pisos según las solicitudes. |
| | 1.3 | El sistema detendrá el motor al llegar al piso solicitado. |
| **Interfaz de usuario** | 2.1 | El sistema contará con botoneras internas y externas para seleccionar pisos. |
| | 2.2 | El sistema mostrará en pantalla LCD el piso actual y mensajes de estado. |
| | 2.3 | El sistema notificará visualmente errores o estados de espera. |
| **Seguridad** | 3.1 | El sistema contará con sensores de puerta y sobrecarga. |
| | 3.2 | El sistema impedirá el movimiento si la puerta está abierta o hay sobrepeso. |
| | 3.3 | El sistema contará con un botón de emergencia para detener el ascensor. |

*Tabla 2.1: Requisitos del proyecto*

### 2.2 Casos de Uso

#### Tabla 2.2: Caso de uso 1 — Solicitud desde un piso
| Elemento | Definición |
| :--- | :--- |
| **Disparador** | El usuario presiona el botón de llamada en un piso. |
| **Precondiciones** | El sistema está encendido y el ascensor detenido en otro piso. |
| **Flujo principal** | El microcontrolador registra la solicitud, activa el motor y mueve el ascensor hasta el piso solicitado. La pantalla LCD muestra el movimiento y el piso actual. Al llegar, se detiene el motor y se habilita la apertura de puerta. |
| **Flujos alternativos**| **a.** El sensor de puerta detecta apertura durante el movimiento, se detiene el motor.<br>**b.** Se detecta sobrecarga, se bloquea el ascensor y se muestra alerta en pantalla. |

#### Tabla 2.3: Caso de uso 2 — Selección de piso en cabina
| Elemento | Definición |
| :--- | :--- |
| **Disparador** | El usuario presiona el botón del piso deseado dentro de la cabina. |
| **Precondiciones** | La puerta está cerrada y el ascensor está detenido. |
| **Flujo principal** | El sistema valida la solicitud, activa el motor y desplaza el ascensor al piso seleccionado. La pantalla LCD muestra el piso actual y el sentido del movimiento. |
| **Flujos alternativos**| **a.** Se presiona el botón de emergencia, deteniendo el motor.<br>**b.** Se abre la puerta durante el movimiento, se activa el protocolo de seguridad. |

#### Tabla 2.4: Caso de uso 3 — Activación del botón de emergencia
| Elemento | Definición |
| :--- | :--- |
| **Disparador** | El usuario presiona el botón de emergencia durante el funcionamiento. |
| **Precondiciones** | El sistema está en movimiento o detenido entre pisos. |
| **Flujo principal** | El microcontrolador detiene inmediatamente el motor y muestra en pantalla un mensaje de alerta. El sistema permanece bloqueado hasta que se reinicie manualmente. |
| **Flujos alternativos**| **a.** El botón se presiona por error, se reinicia el sistema sin consecuencias.<br>**b.** Se detecta sobrecarga simultánea, se mantiene el bloqueo hasta resolver la condición. |
