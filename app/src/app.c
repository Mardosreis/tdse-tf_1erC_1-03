/* Project includes. */
#include "main.h"
#include "task_lcd.h"
/* Demo includes. */
#include "logger.h"
#include "dwt.h"

/* Application & Tasks includes. */
#include "app.h"
#include "board.h"
#include "task_adc.h"
#include "task_pwm.h"
#include "task_hx711.h"
#include "task_rc522.h"
#include "task_hm10.h"

/********************** macros and definitions *******************************/

#define G_APP_CNT_INI		0ul
#define G_APP_TICK_CNT_INI	0ul

#define TASK_X_WCET_INI		0ul

typedef struct {
	void (*task_init)(void *);
	void (*task_update)(void *);
	void *parameters;
} task_cfg_t;

typedef struct {
    uint32_t WCET;
} task_dta_t;

/********************** internal data declaration ****************************/

shared_data_type shared_data;

const task_cfg_t task_cfg_list[]	= {
		{task_adc_init, task_adc_update, &shared_data},
		{task_pwm_init,	task_pwm_update, &shared_data},
};

#define TASK_QTY (sizeof(task_cfg_list)/sizeof(task_cfg_t))

/********************** internal data definition *****************************/

const char *p_sys	= " Bare Metal - Event-Triggered Systems (ETS)\n";
const char *p_app	= " ADC + PWM\n";

/********************** external data declaration *****************************/

uint32_t g_app_cnt;
uint32_t g_app_time_us;

volatile uint32_t g_app_tick_cnt;
task_dta_t task_dta_list[TASK_QTY];

/********************** external functions definition ************************/

void app_init(void)
{
	uint32_t index;
	task_hx711_init(NULL);
	task_rc522_init(NULL);
	task_hm10_init(NULL);

	/* Print out: Application Initialized */
	LOGGER_LOG("\n");
	LOGGER_LOG("%s is running - Tick [mS] = %lu\r\n", GET_NAME(app_init), HAL_GetTick());

	LOGGER_LOG(p_sys);
	LOGGER_LOG(p_app);

	g_app_cnt = G_APP_CNT_INI;

	/* Print out: Application execution counter */
	LOGGER_LOG(" %s = %lu\n", GET_NAME(g_app_cnt), g_app_cnt);

	/* Go through the task arrays */
	for (index = 0; TASK_QTY > index; index++)
	{
		/* Run task_x_init */
		(*task_cfg_list[index].task_init)(task_cfg_list[index].parameters);

		/* Init variables */
		task_dta_list[index].WCET = TASK_X_WCET_INI;
	}

	// Forzamos un piso por defecto para que la memoria nunca arranque en "0"
	    shared_data.piso_actual = 1;
	task_lcd_init(NULL);
	cycle_counter_init();

}

void app_update(void)
{
	uint32_t index;
	uint32_t time_us;

    // --- SENSORES DE PISO ---
	if (HAL_GPIO_ReadPin(REED_PISO1_GPIO_Port, REED_PISO1_Pin) == GPIO_PIN_RESET) {
	    shared_data.piso_actual = 1;
	}
	else if (HAL_GPIO_ReadPin(REED_PISO2_GPIO_Port, REED_PISO2_Pin) == GPIO_PIN_RESET) {
	    shared_data.piso_actual = 2;
	}
	else if (HAL_GPIO_ReadPin(REED_PISO3_GPIO_Port, REED_PISO3_Pin) == GPIO_PIN_RESET) {
	    shared_data.piso_actual = 3;
	}

	/* Check if it's time to run tasks */
	if (G_APP_TICK_CNT_INI < g_app_tick_cnt)
    {
    	g_app_tick_cnt--;

    	/* Update App Counter */
    	g_app_cnt++;
    	g_app_time_us = 0;

		/* Go through the task arrays */
		for (index = 0; TASK_QTY > index; index++)
		{
			cycle_counter_reset();

			/* Run task_x_update */
			(*task_cfg_list[index].task_update)(task_cfg_list[index].parameters);

			time_us = cycle_counter_time_us();

			/* Update variables */
			g_app_time_us += time_us;

			if (task_dta_list[index].WCET < time_us)
			{
				task_dta_list[index].WCET = time_us;
			}
		}

		// 1. Ejecutamos las tareas sueltas pasándole el puntero a los datos
		task_hx711_update(&shared_data);
		task_rc522_update(&shared_data);
		task_hm10_update(&shared_data);

		/// --- CALIBRACIÓN DE LA BALANZA ---

        // 1. Actualizamos la tara al valor real que te tira hoy tu placa en reposo
        static int32_t tara = 9020400;

        int32_t peso_neto = shared_data.peso_actual - tara;

        // 2. GUARDAMOS el cálculo en la memoria compartida (esto arregla el 0.00)
        shared_data.peso_kg = (float)peso_neto / 13000.0;

        // 3. Chequeamos la sobrecarga usando la variable compartida
        if (shared_data.peso_kg > 0.95) {
            shared_data.estado_actual = ESTADO_SOBRECARGA;
        }

        // --- LECTURA DE LOS NUEVOS CONTROLES ---
        bool boton_emerg = (HAL_GPIO_ReadPin(BOTON_GPIO_Port, BOTON_Pin) == GPIO_PIN_RESET);
        bool dip_bypass = (HAL_GPIO_ReadPin(SWICH1_GPIO_Port, SWICH1_Pin) == GPIO_PIN_RESET);
        bool dip_silencio = (HAL_GPIO_ReadPin(SWICH2_GPIO_Port, SWICH2_Pin) == GPIO_PIN_RESET);

		// --- 2. MÁQUINA DE ESTADOS DEL ASCENSOR ---

        // A. Chequeo de seguridad constante (Prioridad Máxima)
        if (boton_emerg) {
            if (shared_data.estado_actual != ESTADO_EMERGENCIA) {
                shared_data.estado_actual = ESTADO_EMERGENCIA;
                // Clavamos los frenos del motor instantáneamente
                HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
                shared_data.pwm_active = 0;

                // Mandamos el mensaje por Bluetooth al celular
                LOGGER_LOG("\r\nLLAMANDO TECNICO DE ASCENSOR FIUBA\r\n");
            }
        }
        // Si sueltan el botón de emergencia, reiniciamos el sistema
        else if (!boton_emerg && shared_data.estado_actual == ESTADO_EMERGENCIA) {
            shared_data.estado_actual = ESTADO_ESPERANDO_LLAVE;
        }
        // Chequeo de Sobrecarga
                else if (shared_data.peso_kg > 0.95 && shared_data.estado_actual != ESTADO_EMERGENCIA) {
                    shared_data.estado_actual = ESTADO_SOBRECARGA;
                }
                // Histéresis de sobrecarga
                else if (shared_data.estado_actual == ESTADO_SOBRECARGA && shared_data.peso_kg < 0.85) {
                    shared_data.estado_actual = ESTADO_ESPERANDO_LLAVE;
                }

        // B. Lógica de transiciones
        switch(shared_data.estado_actual) {

            case ESTADO_ESPERANDO_LLAVE:
                // Si pasaron la tarjeta O si el DIP Switch de Bypass está activado
                if (shared_data.tarjeta_leida || dip_bypass) {
                    shared_data.tarjeta_leida = false;
                    shared_data.estado_actual = ESTADO_ESPERANDO_BOTON;
                }
                break;

            case ESTADO_ESPERANDO_BOTON:
                if (shared_data.boton_piso_presionado) {
                    shared_data.boton_piso_presionado = false;
                    shared_data.estado_actual = ESTADO_MOVIENDO;
                }
                break;

            case ESTADO_MOVIENDO:
                if (shared_data.piso_destino > shared_data.piso_actual) {
                    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
                }
                else if (shared_data.piso_destino < shared_data.piso_actual) {
                    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_SET);
                }
                shared_data.pwm_active = 1;

                if (shared_data.piso_actual == shared_data.piso_destino) {
                    HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
                    shared_data.pwm_active = 0;
                    shared_data.boton_piso_presionado = false;
                    shared_data.estado_actual = ESTADO_ESPERANDO_LLAVE;
                }
                break;

            case ESTADO_SOBRECARGA:
            case ESTADO_EMERGENCIA:
                HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, GPIO_PIN_RESET);
                shared_data.pwm_active = 0;
                break;

            case ESTADO_SET_UP:
                {
                    static uint32_t contador_menu = 0;
                    contador_menu++;
                    if (contador_menu >= 1000) {
                        LOGGER_LOG("\r\n--- MODO SET-UP (MENU INTERACTIVO) ---\r\n");
                        LOGGER_LOG("Envie 'T' -> Calibrar Cero de la balanza (Tara actual: %ld)\r\n", tara);
                        LOGGER_LOG("Envie 'S' -> Guardar y Salir\r\n");
                        contador_menu = 0;
                    }
                    if (shared_data.flag_tarar) {
                                            tara = shared_data.peso_actual;
                                            shared_data.flag_tarar = false;

                                            // Encendemos el cronómetro del LCD por 3 segundos
                                            shared_data.timer_tara = 15;

                                            LOGGER_LOG("\r\n[!] BALANZA CALIBRADA EXITOSAMENTE [!]\r\n");
                                        }
                }
                break;
        }

        // --- C. CONTROL DEL BUZZER ---
                // Suena SOLO si hay un error Y el DIP de silencio NO está activado
                if ((shared_data.estado_actual == ESTADO_SOBRECARGA || shared_data.estado_actual == ESTADO_EMERGENCIA) && !dip_silencio) {
                    // Hacemos "Toggle" (prender y apagar) muy rápido para crear la nota musical
                    HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
                } else {
                    // Silencio absoluto
                    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
                }

                // --- 3. LOS ACTUADORES REACCIONAN (CON FRENO DE REFRESCO) ---
                        static uint32_t contador_refresco = 0;
                        contador_refresco++;

                        if (contador_refresco >= 200) {
                            task_lcd_update(&shared_data);

                            // ACÁ ESTÁ LA LÍNEA MODIFICADA PARA VER EL VALOR RAW (CRUDO) Y EL PESO EN KG
                            LOGGER_LOG("RAW: %ld | Peso: %.2f Kg\r\n", shared_data.peso_actual, shared_data.peso_kg);

                            contador_refresco = 0;
                        }

                        // --- ACTUALIZACIÓN DE LEDs DE PISO ---
                        // Solo encendemos el pin si coincide con el piso actual. Mucho más estable eléctricamente.
                        HAL_GPIO_WritePin(LED_PISO1_GPIO_Port, LED_PISO1_Pin, (shared_data.piso_actual == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(LED_PISO2_GPIO_Port, LED_PISO2_Pin, (shared_data.piso_actual == 2) ? GPIO_PIN_SET : GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(LED_PISO3_GPIO_Port, LED_PISO3_Pin, (shared_data.piso_actual == 3) ? GPIO_PIN_SET : GPIO_PIN_RESET);

                    } // <-- Cierra el "if (G_APP_TICK_CNT_INI < g_app_tick_cnt)"
                } // <-- Cierra la función app_update()

                void HAL_SYSTICK_Callback(void)
                {
                    g_app_tick_cnt++;
                }
