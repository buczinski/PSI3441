#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS 500

// Define os LEDs usando Device Tree
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)

// Extrai as configurações do Device Tree
#if DT_NODE_HAS_STATUS(LED0_NODE, okay) || DT_NODE_HAS_STATUS(LED1_NODE, okay) || DT_NODE_HAS_STATUS(LED2_NODE, okay)
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);    
#else
#error "Unsupported board: leds devicetree alias is not defined"
#endif

// Definição dos estados possíveis para a máquina de estados
typedef enum {
    STATE_LED0_ON,
    STATE_LED1_ON,
    STATE_LED2_ON
} led_state_t;

void main(void)
{
    // Verifica se os devices estão prontos
    if (!gpio_is_ready_dt(&led0) || !gpio_is_ready_dt(&led1) || !gpio_is_ready_dt(&led2)) {
        printk("Error: One or more LED devices are not ready\n");
        return;
    }

    // Configura os 3 pinos como saída e inicia DESLIGADOS (INACTIVE)
    gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led2, GPIO_OUTPUT_INACTIVE);

    printk("LEDs ready to blink using a State Machine!\n");

    // Inicializa a máquina de estados no primeiro LED
    led_state_t current_state = STATE_LED0_ON;

    while (1) {
        // Desliga todos os LEDs a cada ciclo para garantir que apenas o LED do estado atual fique aceso
        gpio_pin_set_dt(&led0, 0);
        gpio_pin_set_dt(&led1, 0);
        gpio_pin_set_dt(&led2, 0);

        // Avalia o estado atual, executa a ação e define o próximo estado
        switch (current_state) {
            case STATE_LED0_ON:
                gpio_pin_set_dt(&led0, 1);
                current_state = STATE_LED1_ON; // Transição
                break;

            case STATE_LED1_ON:
                gpio_pin_set_dt(&led1, 1);
                current_state = STATE_LED2_ON; // Transição
                break;

            case STATE_LED2_ON:
                gpio_pin_set_dt(&led2, 1);
                current_state = STATE_LED0_ON; // Retorna ao início
                break;
        }

        // Aguarda antes de processar o próximo estado
        k_msleep(SLEEP_TIME_MS);
    }
}