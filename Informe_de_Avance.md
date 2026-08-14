# <span style="color: #1F618D;">Informe de Avances del TP Final</span>

*Autores:* Mariana dos Reis - Padrón 111545
           Uma Toscan - Padrón 111106
           Felipe Colodro - Padrón 106433
*Fecha:* 13/08/26
*Cuatrimestre:* 1er cuatrimestre 2026

A continuación, se detalla el informe de avances del TP final a partir de los requerimientos.

### <span style="color: #2874A6;">Estado de Implementación</span>
- 🟢 Ya implementado
- 🟡 En proceso de implementarse
- 🔴 No se implementará

---

### <span style="color: #2874A6;">Tabla 2.1: Requisitos del proyecto</span>

| Grupo | ID | Descripción | Estado |
| :--- | :--- | :--- | :---: |
| *Control de movimiento* | 1.1 | El sistema controlará el motor del ascensor mediante señales PWM. | 🟡 |
| | 1.2 | El sistema permitirá el desplazamiento entre pisos según las solicitudes. | 🟡 |
| | 1.3 | El sistema detendrá el motor al llegar al piso solicitado. | 🟡 |
| *Interfaz de usuario* | 2.1 | El sistema contará con botoneras internas y externas para seleccionar pisos. | 🟡 |
| | 2.2 | El sistema mostrará en pantalla LCD el piso actual y mensajes de estado. | 🟡 |
| | 2.3 | El sistema notificará visualmente errores o estados de espera. | 🟡 |
| *Seguridad* | 3.1 | El sistema contará con sensores de puerta y sobrecarga. | 🟡 |
| | 3.2 | El sistema impedirá el movimiento si la puerta está abierta o hay sobrepeso. | 🟡 |
| | 3.3 | El sistema contará con un botón de emergencia para detener el ascensor. | 🟡 |

<br>

### <span style="color: #2874A6;">Tabla 2.2: Caso de uso 1 — Solicitud desde un piso</span>

| Elemento | Definición |
| :--- | :--- |
| *Disparador* | El usuario presiona el botón de llamada en un piso. |
| *Precondiciones* | El sistema está encendido y el ascensor detenido en otro piso. |
| *Flujo principal* | El microcontrolador registra la solicitud, activa el motor y mueve el ascensor hasta el piso solicitado. La pantalla LCD muestra el movimiento y el piso actual. Al llegar, se detiene el motor y se habilita la apertura de puerta. |
| *Flujos alternativos* | *a.* El sensor de puerta detecta apertura durante el movimiento, se detiene el motor.<br>*b.* Se detecta sobrecarga, se bloquea el ascensor y se muestra alerta en pantalla. |

<br>

### <span style="color: #2874A6;">Tabla 2.3: Caso de uso 2 — Selección de piso en cabina</span>

| Elemento | Definición |
| :--- | :--- |
| *Disparador* | El usuario presiona el botón del piso deseado dentro de la cabina. |
| *Precondiciones* | La puerta está cerrada y el ascensor está detenido. |
| *Flujo principal* | El sistema valida la solicitud, activa el motor y desplaza el ascensor al piso seleccionado. La pantalla LCD muestra el piso actual y el sentido del movimiento. |
| *Flujos alternativos* | *a.* Se presiona el botón de emergencia, deteniendo el motor. |

<br>

### <span style="color: #2874A6;">Tabla 2.4: Caso de uso 3 — Activación del botón de emergencia</span>

| Elemento | Definición |
| :--- | :--- |
| *Disparador* | El usuario presiona el botón de emergencia durante el funcionamiento. |
| *Precondiciones* | El sistema está en movimiento o detenido entre pisos. |
| *Flujo principal* | El microcontrolador detiene inmediatamente el motor y muestra en pantalla un mensaje de alerta. El sistema permanece bloqueado hasta que se reinicie manualmente. |
| *Flujos alternativos* | *a.* El botón se presiona por error, se reinicia el sistema sin consecuencias.<br>*b.* Se detecta sobrecarga simultánea, se mantiene el bloqueo hasta resolver la condición. |
