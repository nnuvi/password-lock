/*
 * keypad.c
 */
#include "keypad.h"\

#define ROW_PORT GPIOA
#define COL_PORT GPIOA

int row_pins[4] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3};
int col_pins[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};

char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char get_key(void)
{
    for (int row = 0; row < 4; row++)
    {
        // Set all rows HIGH
        for (int r = 0; r < 4; r++)
        {
            HAL_GPIO_WritePin(ROW_PORT, row_pins[r], GPIO_PIN_SET);
        }

        // Pull current row LOW
        HAL_GPIO_WritePin(ROW_PORT, row_pins[row], GPIO_PIN_RESET);

        // Scan columns
        for (int col = 0; col < 4; col++)
        {
            if (HAL_GPIO_ReadPin(COL_PORT, col_pins[col]) == GPIO_PIN_RESET)
            {
                HAL_Delay(200); // debounce
                while (HAL_GPIO_ReadPin(COL_PORT, col_pins[col]) == GPIO_PIN_RESET); // wait for release
                return keymap[row][col];
            }
        }
    }
    return 0;
}
