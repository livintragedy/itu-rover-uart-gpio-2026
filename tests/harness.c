#define main Firmware_Main
#include "../Rover_UART_GPIO/Core/Src/main.c"
#undef main
static CommandParser parser;
static uint8_t pins;
void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state)
{
    (void)port;
    uint8_t mask = pin == GPIO_PIN_5 ? 1U : 2U;
    if (state == GPIO_PIN_SET) pins |= mask; else pins &= (uint8_t)~mask;
}
void HAL_GPIO_TogglePin(GPIO_TypeDef *port, uint16_t pin)
{
    (void)port;
    pins ^= pin == GPIO_PIN_5 ? 1U : 2U;
}
void Test_Reset(uint32_t initial)
{
    pins = (uint8_t)initial;
    Command_Reset(&parser);
}
uint32_t Test_Byte(uint32_t value)
{
    uint8_t command[2];
    if (Command_Feed(&parser, (uint8_t)value, command))
    {
        ApplyPin(GPIO_PIN_5, command[0]);
        ApplyPin(GPIO_PIN_6, command[1]);
        return 1U;
    }
    return 0U;
}
uint32_t Test_State(void) { return pins; }
