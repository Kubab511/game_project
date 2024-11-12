#include <stm32f031x6.h>
#include <stdbool.h>

bool is_left_button_pressed() {
    if ((GPIOB->IDR & (1 << 5)) == 0) {
        return true;
    }
    return false;
}

bool is_right_button_presesd() {
    if ((GPIOB->IDR & (1 << 4)) == 0) {
        return true;
    }
    return false;
}

bool is_down_button_pressed() {
    if ((GPIOA->IDR & (1 << 11)) == 0) {
        return true;
    }
    return false;
}

bool is_up_button_pressed() {
    if ((GPIOA->IDR & (1 << 8)) == 0) {
        return true;
    }
    return false;
}