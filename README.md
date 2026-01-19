# 🐍 Snake Game on STM32
Embedded Systems Project – STM32 + ST7735 LCD

---

## General Description

This project represents a complete implementation of the classic **Snake game** on an STM32 microcontroller using a **128x128 ST7735 TFT LCD** display.  
The entire game logic, rendering, input handling, timing, and hardware interaction are implemented manually in C, without using any game engine or operating system.

The purpose of this project was to deeply understand:
- how a microcontroller interacts with external hardware,
- how to structure a medium-size embedded application,
- how to manage real-time constraints,
- how to design game logic using only low-level tools.

All functionality is implemented inside `main.c`, together with external drivers for the LCD and graphics primitives.

---

## Hardware Architecture

The hardware setup consists of:
- STM32 microcontroller
- ST7735 SPI TFT display (128x128)
- 4 directional push buttons
- UART interface for debugging
- Timer used as a time base
- SPI interface used for display communication

The LCD is driven via SPI in **master half-duplex mode**, since the display only receives data and never sends anything back.

![Hardware setup](https://github.com/user-attachments/assets/80f3c1ab-0ee9-4121-a79b-d18fb734b717)

---

## Software Architecture Overview

The software is organized around the following main components:
- low-level peripheral initialization (GPIO, SPI, TIM, UART),
- graphical rendering functions,
- snake data structure and movement logic,
- collision detection,
- food spawning logic,
- interrupt-based input handling,
- timer-based pacing of the game.

The main loop does **not** poll buttons continuously. Instead, all user input is handled using **external interrupts**, making the game more responsive and efficient.

---

## Screen Coordinate System and Grid Logic

The LCD has a resolution of **128x128 pixels**.  
To simplify game logic, the entire screen is divided into a **16x16 logical grid**, where each grid cell is **8x8 pixels**.

This means:
- all snake segments are aligned to multiples of 8,
- food always spawns on valid grid positions,
- collision detection becomes trivial (coordinate equality).

Valid coordinate values are:
0, 8, 16, 24, ..., 120

yaml
Copy code

This design choice drastically simplifies movement, drawing, and collision checks.

---

## Snake Data Structure

The snake is implemented as an array of structures:

```c
typedef struct {
    int16_t x;
    int16_t y;
} Sarpe;

Sarpe sarpe[255];
Each element of the array represents one segment of the snake.
The snake length is controlled using the variable scor.

c
Copy code
uint8_t scor = 2; // initial length = 3 segments (0,1,2)
The choice of int16_t instead of uint8_t was deliberate, to avoid underflow issues when subtracting values during movement.

Rendering the Snake
Each snake segment is rendered as an 8x8 pixel square.
Rendering is done pixel-by-pixel using nested loops.

c
Copy code
void pixel_sarpe(int16_t x, int16_t y) {
    for(uint8_t i = x; i < x + 8; i++) {
        for(uint8_t j = y; j < y + 8; j++) {
            ST7735_DrawPixel(i, j, ST7735_GREEN);
        }
    }
}
Before each movement update, the snake is erased by drawing the same area using the background color (black).
This gives the illusion of smooth movement without flickering.

The snake head is drawn differently, including eyes, purely for visual feedback.

Snake Movement Logic (Shift Algorithm)
Snake movement is implemented using a backward shift algorithm.

The idea is simple:

each segment takes the position of the previous one,

the head is moved last, based on the current direction.

c
Copy code
for(uint8_t i = scor; i > 0; i--) {
    sarpe[i].x = sarpe[i - 1].x;
    sarpe[i].y = sarpe[i - 1].y;
}
After shifting the body, the head position is updated:

right: x += 8

left: x -= 8

up: y -= 8

down: y += 8

Screen wrapping is implemented, so when the snake exits one side of the screen, it reappears on the opposite side.

Food Spawning Logic
Food is spawned at random grid-aligned coordinates:

c
Copy code
randx = (rand() % 16) * 8;
randy = (rand() % 16) * 8;
Before placing the food, the algorithm checks that the position does not overlap with any snake segment.

c
Copy code
for(uint8_t i = 0; i <= scor; i++) {
    if(sarpe[i].x == randx && sarpe[i].y == randy) {
        ok = 1;
        break;
    }
}
Randomness is ensured by reseeding the generator using the system tick:

c
Copy code
srand(HAL_GetTick());
Snake Growth Logic
When the snake eats food:

the score is incremented,

a new segment is added at the tail.

The position of the new segment is calculated based on the direction of the last two segments, ensuring that the snake grows naturally and does not visually break.

This part is intentionally more complex, because simply duplicating coordinates would cause overlapping segments.

Collision Detection
Self-Collision
At each update, the head is compared against all body segments:

c
Copy code
for(uint8_t i = 1; i <= scor; i++) {
    if(sarpe[0].x == sarpe[i].x && sarpe[0].y == sarpe[i].y) {
        game_over = 1;
    }
}
If a collision is detected, the game ends immediately.

Input Handling Using External Interrupts
Directional buttons are connected to GPIO pins configured as EXTI inputs.
Each button generates an interrupt on a rising edge.

c
Copy code
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(timp >= 15) {
        // change direction safely
        timp = 0;
    }
}
A small timer-based delay is used to debounce the buttons and prevent unrealistic instant direction changes.

Opposite directions are blocked to prevent the snake from reversing into itself.

Timer Usage
A hardware timer is configured to generate periodic interrupts.
The timer increments a global variable used for:

debouncing input,

controlling game speed,

ensuring consistent gameplay regardless of main loop execution time.

Game Over Screen
When the game ends, the screen is filled using a red background animation and the final score is displayed.

The score shown is adjusted so that it starts from zero, excluding the initial snake length.

Visual Results







Conclusion
This project demonstrates a complete embedded application that integrates:

low-level hardware configuration,

SPI communication,

interrupt-driven input,

timer-based logic,

real-time graphics rendering,

and non-trivial game mechanics.

The Snake game is fully deterministic, efficient, and well suited for a constrained embedded environment, making it an excellent educational example of real-world embedded software development.
