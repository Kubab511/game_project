#include <stm32f031x6.h>
#include <stdbool.h>
#include "buttons.h"
#include "display.h"
#include "serial.h"
#include <stdlib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

// Function declarations
void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void main_menu();
uint16_t random_range(uint16_t min, uint16_t max);
void game_over(int score);

// Global variable to keep track of milliseconds
volatile uint32_t milliseconds;

// Constant arrays for display graphics
const uint16_t truck[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4228,16255,4228,4228,16255,4228,0,0,0,0,0,0,4228,33020,33020,33020,33020,4228,0,0,0,0,0,0,4228,33020,33020,33020,33020,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,4228,7936,4228,4228,7936,4228,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint16_t truck_lr[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,16255,33020,33020,4228,4228,4228,4228,4228,4228,4228,4228,7936,4228,33020,33020,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,33020,33020,4228,4228,4228,4228,4228,4228,4228,4228,4228,16255,33020,33020,4228,4228,4228,4228,4228,4228,4228,4228,7936,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,4228,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint16_t package[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62820,62820,62820,62820,62820,62820,0,0,0,0,0,0,62820,62820,62820,62820,62820,8956,0,0,0,0,0,0,62820,8956,62820,62820,8956,62820,0,0,0,0,0,0,62820,62820,8956,8956,62820,62820,0,0,0,0,0,0,62820,62820,62820,62820,62820,62820,0,0,0,0,0,0,62820,62820,62820,62820,62820,62820,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

const uint16_t black[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

int main(void) {
    // Variable declarations
    int hinverted = 0;
    int vinverted = 0;
    int hmoved = 0;
    int vmoved = 0;
    int score = 0;
    //timer is multiplied by 20 as the loop runs every 50ms
    int timer = 600;
    uint16_t x = 50;
    uint16_t y = 50;
    uint16_t oldx = x;
    uint16_t oldy = y;
    uint16_t package_x = random_range(0, SCREEN_WIDTH - 12);
    uint16_t package_y = random_range(30, SCREEN_HEIGHT - 16);

    // Initialize system
    initClock();
    initSysTick();
    setupIO();
    initSerial();

    while(!(is_left_button_pressed() || is_right_button_presesd() || is_down_button_pressed() || is_up_button_pressed())) {
        main_menu();
    }

    start:
    //overwrite the main menu text with a black rectangle
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    // Display initial image
    putImage(package_x, package_y, 12, 16, package, 0, 0);

    //reset score and timer
    score = 0;
    timer = 600;

    while (timer > 0) {
        // Reset movement flags
        hmoved = vmoved = 0;
        hinverted = vinverted = 0;

        // Check if right button is pressed
        if (is_right_button_presesd()) {
            if (x < 110) {
                x = x + 1;
                hmoved = 1;
                hinverted = 1;
            }
        }

        // Check if left button is pressed
        if (is_left_button_pressed()) {
            if (x > 0) {
                x = x - 1;
                hmoved = 1;
                hinverted = 0;
            }
        }

        // Check if down button is pressed
        if (is_down_button_pressed()) {
            if (y < 140) {
                y = y + 1;
                vmoved = 1;
                vinverted = 1;
            }
        }

        // Check if up button is pressed
        if (is_up_button_pressed()) {
            if (y > 35) {
                y = y - 1;
                vmoved = 1;
                vinverted = 0;
            }
        }

        // Redraw image if there has been movement
        if ((vmoved) || (hmoved)) {
            fillRectangle(oldx, oldy, 12, 16, 0);
            oldx = x;
            oldy = y;

            if (hmoved) {
                putImage(x, y, 12, 16, truck_lr, hinverted, 0);
            } else {
                putImage(x, y, 12, 16, truck, 0, vinverted);
            }

            // Check for overlap
            if (isInside(package_x, package_y, 12, 16, x, y) || isInside(package_x, package_y, 12, 16, x + 12, y) || isInside(package_x, package_y, 12, 16, x, y + 16) || isInside(package_x, package_y, 12, 16, x + 12, y + 16)) {
                score++;
                if (score < 30) {
                    timer += 100;
                } else {
                    timer += 40;
                }
                putImage(package_x, package_y, 12, 16, black, 0, 0);
                package_x = random_range(0, SCREEN_WIDTH - 12);
                package_y = random_range(30, SCREEN_HEIGHT - 16);
            }
        }

        putImage(package_x, package_y, 12, 16, package, 0, 0);

        //prints the score
        printText("Score:", 10, 10, RGBToWord(0xff, 0xff, 0), 0);
        printNumber(score, 10, 20, RGBToWord(0xff, 0xff, 0), 0);
        //prints the timer
        printText("Time:", SCREEN_WIDTH-45, 10, RGBToWord(0xff, 0xff, 0), 0);
        printNumber(timer/20, SCREEN_WIDTH-45, 20, RGBToWord(0xff, 0xff, 0), 0);
        timer--;
        //print data to serial port
        eputs("Score: ");
        printDecimal(score);
        eputs(" Time: ");
        printDecimal(timer/20);
        eputs("\n");
        // Delay to reduce flicker
        delay(50);
    }

    //clear screen only once so it won't flicker in the loop below
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    //game over screen, allowing the player to restart the game
    while(!(is_left_button_pressed() || is_right_button_presesd() || is_down_button_pressed() || is_up_button_pressed())) {
        game_over(score);
    }
    goto start;

    return 0;
}

uint16_t random_range(uint16_t min, uint16_t max) {
    return (rand() % (max - min + 1)) + min;
}

void main_menu() {
    printText("Welcome to", (SCREEN_WIDTH / 2) - 32, (SCREEN_HEIGHT / 2) - 40, RGBToWord(0xff, 0xff, 0xff), 0);
    printText("Package Delivery", (SCREEN_WIDTH / 2) - 50, (SCREEN_HEIGHT / 2) - 30, RGBToWord(0xff, 0xff, 0xff), 0);
    printText("Press any button", (SCREEN_WIDTH / 2) - 54, (SCREEN_HEIGHT / 2) - 10, RGBToWord(0xff, 0xff, 0xff), 0);
    printText("to start", (SCREEN_WIDTH / 2) - 32, (SCREEN_HEIGHT / 2), RGBToWord(0xff, 0xff, 0xff), 0);
}

void game_over(int score) {
    printText("Game Over", (SCREEN_WIDTH / 2) - 32, (SCREEN_HEIGHT / 2) - 40, RGBToWord(0xff, 0xff, 0xff), 0);
    printText("Score:", (SCREEN_WIDTH / 2) - 16, (SCREEN_HEIGHT / 2) - 20, RGBToWord(0xff, 0xff, 0xff), 0);
    printNumber(score, (SCREEN_WIDTH / 2) - 16, (SCREEN_HEIGHT / 2) - 10, RGBToWord(0xff, 0xff, 0xff), 0);
    printText("Press any button", (SCREEN_WIDTH / 2) - 54, (SCREEN_HEIGHT / 2) + 10, RGBToWord(0xff, 0xff, 0xff), 0);
    printText("to restart", (SCREEN_WIDTH / 2) - 32, (SCREEN_HEIGHT / 2) + 20, RGBToWord(0xff, 0xff, 0xff), 0);
}

// Initialize SysTick timer
void initSysTick(void) {
    SysTick->LOAD = 48000;
    SysTick->CTRL = 7;
    SysTick->VAL = 10;
    __asm(" cpsie i "); // Enable interrupts
}

// SysTick interrupt handler
void SysTick_Handler(void) {
    milliseconds++;
}

// Initialize system clock
void initClock(void) {
    // Disable PLL
    RCC->CR &= ~(1u << 24);
    while ((RCC->CR & (1 << 25))); // Wait for PLL ready to be cleared

    // Configure Flash memory interface
    FLASH->ACR |= (1 << 0);
    FLASH->ACR &= ~((1u << 2) | (1u << 1));
    FLASH->ACR |= (1 << 4); // Enable Flash prefetch buffer

    // Set PLL multiplier to 12 (48MHz)
    RCC->CFGR &= ~((1u << 21) | (1u << 20) | (1u << 19) | (1u << 18));
    RCC->CFGR |= ((1 << 21) | (1 << 19));

    // Set ADC prescaler to 4
    RCC->CFGR |= (1 << 14);

    // Enable PLL
    RCC->CR |= (1 << 24);

    // Set PLL as system clock source
    RCC->CFGR |= (1 << 1);
}

// Delay function
void delay(volatile uint32_t dly) {
    uint32_t end_time = dly + milliseconds;
    while (milliseconds != end_time)
        __asm(" wfi "); // Sleep
}

// Enable pull-up resistor
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber) {
    Port->PUPDR = Port->PUPDR & ~(3u << BitNumber * 2); // Clear pull-up resistor bits
    Port->PUPDR = Port->PUPDR | (1u << BitNumber * 2); // Set pull-up bit
}

// Set pin mode
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode) {
    uint32_t mode_value = Port->MODER;
    Mode = Mode << (2 * BitNumber);
    mode_value = mode_value & ~(3u << (BitNumber * 2));
    mode_value = mode_value | Mode;
    Port->MODER = mode_value;
}

// Check if point is inside rectangle
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py) {
    uint16_t x2, y2;
    x2 = x1 + w;
    y2 = y1 + h;
    int rvalue = 0;
    if ((px >= x1) && (px <= x2)) {
        if ((py >= y1) && (py <= y2))
            rvalue = 1;
    }
    return rvalue;
}

// Setup IO ports
void setupIO() {
    RCC->AHBENR |= (1 << 18) + (1 << 17); // Enable Ports A and B
    display_begin();
    pinMode(GPIOB, 4, 0);
    pinMode(GPIOB, 5, 0);
    pinMode(GPIOA, 8, 0);
    pinMode(GPIOA, 11, 0);
    enablePullUp(GPIOB, 4);
    enablePullUp(GPIOB, 5);
    enablePullUp(GPIOA, 11);
    enablePullUp(GPIOA, 8);
    pinMode(GPIOA, 0, 1); // Make GPIOA bit 0 an output
}