#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "boards.h"

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Configure board. */
    bsp_board_init(BSP_INIT_LEDS);

    /* Store how many times should led blink based on DEVICE ID */
    int led_count[4] = {6, 5, 9, 3};

    /* Toggle LEDs. */
    while (true)
    {
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            for(int j = 0; j < led_count[i]; j++)
            {
                bsp_board_led_on(i);
                nrf_delay_ms(500);
                bsp_board_led_off(i);
                nrf_delay_ms(500);
            }
        }
    }
}

/**
 *@}
 **/
