Snake Game Implementation – Technical Documentation

Embedded Systems Project (STM32, SPI, Graphics LCD)

1. Source Code and Implementation Overview

The entire source code of the project is implemented in main.c. This represents an original software and hardware implementation, developed on a custom PCB, as part of the university project P2 (Project 2).

The application is written in C, using STM32 HAL libraries, and targets the STM32F103CBT6 microcontroller. The project integrates low-level hardware control, real-time graphics rendering, interrupt-based input handling, and deterministic game logic.

2. Software Development Stages

The development process was structured into the following major stages:

STM32 microcontroller firmware and clock configuration

Peripheral configuration (SPI, GPIO, Timers, UART, External Interrupts)

Integration of LCD graphics libraries

Mathematical modeling of the Snake game grid

Data structures and movement logic

Collision detection and scoring logic

Optimization and refactoring using interrupts

Hardware assembly and PCB validation

3. STM32 Microcontroller Software / Firmware Configuration

I configured the STM32F103CBT6 microcontroller to operate at 72 MHz, using an external quartz oscillator, ensuring stable timing and fast instruction execution.

Key Configuration Choices

SPI (Half-Duplex, Master mode) for LCD communication

External interrupts (EXTI) for directional buttons

TIM2 configured to generate 100 interrupts per second

UART used for debugging and coordinate logging

Timer configuration allows me to track time intervals precisely, which is essential for:

debouncing inputs

controlling snake movement speed

synchronizing game logic

4. Libraries Used

I integrated the following libraries into the project:

ST7735 display driver

Adafruit_GFX graphics core

Custom fonts library

These libraries provide essential graphical primitives such as:

pixel drawing

screen filling

text rendering

coordinate-based drawing

The LCD configuration (resolution, orientation, control pins) is defined in ST7735.h.

5. Snake Game Design – Mathematical and Spatial Model

The LCD screen resolution is 128×128 pixels, with the origin (0,0) located in the top-left corner.

To simplify rendering and collision detection, I discretized the screen into a 16×16 grid, where:

each logical cell = 8×8 pixels

valid coordinates ∈ {0, 8, 16, …, 120}

This abstraction allows efficient movement, rendering, and collision checks.

6. Core Data Structures
Snake Representation

The snake is modeled as a vector of structures, each structure representing a body segment:

typedef struct
{
    int16_t x;
    int16_t y;
} Sarpe;

Sarpe sarpe[255];

Design Rationale:

int16_t prevents underflow/overflow issues encountered with uint8_t

array size allows dynamic snake growth

sarpe[0] always represents the head

The snake length is controlled by the variable:

uint8_t scor = 2; // snake starts with 3 segments: 0, 1, 2

7. Rendering Logic (Pixel Generator Concept)

Each snake segment and food item is rendered as an 8×8 pixel square, using a pivot coordinate (x, y):

void pixel_sarpe(int16_t x, int16_t y)
{
    for(uint8_t i = x; i < x + 8; i++)
        for(uint8_t j = y; j < y + 8; j++)
            ST7735_DrawPixel(i, j, ST7735_GREEN);
}


Food is rendered similarly but using a different color:

void pixel_hrana(int16_t x, int16_t y)
{
    for(uint8_t i = x; i < x + 8; i++)
        for(uint8_t j = y; j < y + 8; j++)
            ST7735_DrawPixel(i, j, ST7735_RED);
}

8. Snake Movement Logic (Vector Shifting)

Snake movement is implemented using a backward vector shift, followed by updating the head position.

Example – movement to the right:

for(uint8_t i = scor; i > 0; i--)
{
    sarpe[i] = sarpe[i - 1];
}

if(sarpe[0].x + 8 == 128)
    sarpe[0].x = 0;   // wrap through wall
else
    sarpe[0].x += 8;


This approach:

preserves body continuity

avoids dynamic memory allocation

is computationally efficient

9. Collision Detection
Self-Collision
for(uint8_t i = 1; i <= scor; i++)
{
    if(sarpe[0].x == sarpe[i].x &&
       sarpe[0].y == sarpe[i].y)
    {
        game_over = 1;
        interfata_game_over();
    }
}

Wall Collision (Removed in Later Versions)

Initially, wall collisions ended the game. Later, I implemented screen wrapping, allowing the snake to pass through walls and reappear on the opposite side.

10. Food Generation (Randomized, Collision-Safe)

Food coordinates are generated using:

randx = (rand() % 16) * 8;
randy = (rand() % 16) * 8;


To ensure randomness, I use:

srand(HAL_GetTick());


The algorithm verifies that food does not spawn on the snake’s body before final placement.

11. Snake Growth Logic

When the snake consumes food:

if(sarpe[0].x == randx && sarpe[0].y == randy)
{
    scor++;
}


The new segment is added based on the direction of the tail, determined by comparing the last two segments:

if(sarpe[scor-1].x + 8 == sarpe[scor-2].x)
{
    sarpe[scor].x = sarpe[scor-1].x - 8;
    sarpe[scor].y = sarpe[scor-1].y;
}


This guarantees correct spatial continuity.

12. Input Handling via External Interrupts

Buttons are configured as EXTI interrupts, eliminating polling delays:

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(timp >= 15) // debounce
    {
        if(GPIO_Pin == GPIO_PIN_14)
            apasat14 = 1; // move right
        timp = 0;
    }
}


This approach significantly improves responsiveness and game feel.

13. Game Over Interface












The game over screen is rendered dynamically using an 8×8 pixel fill algorithm and displays the final score centrally.

14. Optimizations and Enhancements

External interrupts instead of polling

Debouncing handled in software

Snake head rendered with eyes for visual clarity

Vivid color palette for improved UX

Wall wrapping for increased gameplay complexity

15. Conclusion

This project represents a complete embedded game implementation, combining:

low-level hardware control

real-time graphics

interrupt-driven input

deterministic game physics

The solution is efficient, scalable, and suitable for further extensions such as multiplayer, sound output, or AI-controlled agents.
