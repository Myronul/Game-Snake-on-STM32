Source Code and Implementation

The source code is located in main.c. This is an original implementation using a custom PCB, developed as part of a university project called P2 (Project 2).
Game Operation
The implemented Snake game operates through interaction between the STM32 microcontroller and the display, utilizing serial SPI communication to transmit display data. The microcontroller continuously updates the LCD's display buffer, drawing game elements such as the snake and food based on the current game state and user inputs. Input from the buttons is read by the microcontroller and interpreted as movement commands for the snake (each button has implemented control logic that assigns a direction: up/down/left/right).
In parallel, the microcontroller performs continuous checks to detect collisions between the snake and the screen edges (comparing the snake head's position with known screen boundaries) or its own body (comparing the head's position with the positions of body segments). To optimize these checks, the positions of the head and snake body segments are stored in a list/array. If a collision is detected (compared positions are equal), the game ends and the final score is displayed on screen.
Subsequent modification: Desiring to create a more interactive and complex game, we abandoned the idea of ending the game upon collision with screen edges. Thus, the snake is capable of passing through walls and appearing discontinuously on the opposite side of the screen, continuing its movement normally; the only way the game ends is through the snake colliding with itself.
Food is generated at a random position on the screen, defining a valid zone where it can appear (the entire screen surface minus the space already occupied by the snake's body or previously generated food). Two coordinates (x,y) are randomly generated within the defined valid space. With each update of the snake's position, the intersection of the head with the food is checked, and in case of a match, its length increases and the score is updated.
The scoring logic is implemented to track the user's performance throughout the game. At the start of the game, the score is initialized to zero. When the snake consumes food, the player's score is increased (each piece of food consumed brings a certain number of points, and the score is incremented by a fixed value). The score is displayed on screen in a corner in real-time, and appears centrally at the end.
Problems Encountered During Implementation
Throughout software and hardware development, we encountered various problems such as:

The display not functioning in the VSCode working environment.
Finding an efficient method for soldering physical connections while avoiding short circuits.
Implementing an efficient method for adding new snake segments after consuming food.
Generating food position (RNG) and avoiding collision with the snake's body.
Resolving an overflow issue for an unsigned 8-bit variable that decremented below the minimum allowed value (0).
In the loop for verifying food position correctness, we omitted comparing with the snake head's position (we used <0 instead of <=0).

![poza3](https://github.com/user-attachments/assets/1a17568f-accf-4e28-bca0-9d80c785ead5)

![poza2](https://github.com/user-attachments/assets/650d2de1-cf6e-4891-93fd-78db88ff24d5)

![poza4](https://github.com/user-attachments/assets/3c9507b5-6640-40a8-be79-9d81f9b52de5)

![poza6](https://github.com/user-attachments/assets/b7be36ce-8378-49a2-b540-ee51da60b0bd)

![poza7](https://github.com/user-attachments/assets/12bf221f-bf74-43c2-b310-dd2fdb917c31)

![poza8](https://github.com/user-attachments/assets/123239aa-e898-43a8-a774-5c7c8dd0a171)



Optimizations Made During Implementation
Regarding optimization, we added external interrupts for inputs, ensuring that pull-down is respected and that interrupts are checked on the rising edge. Additionally, we stylized the game by adding eyes to the snake, making it more attractive. We used vivid colors in the game design to improve users' visual experience. Furthermore, the snake can pass through walls, increasing the complexity level of both the game and software development.Vreau sa iei tot ce ti am trimis eu, absolut tot, sa le pui cap la cap si sa le scrii in egnelza la persoana 1 cu Me mylesf I no We, dar ia tot ce ti am trimis frte12:58 AMSnake Game Implementation - Technical Documentation
Source Code and Implementation Overview
The source code is located in main.c. This is an original implementation using a custom PCB, developed as part of a university project called P2 (Project 2).
Software Development Stages
The software development stages are as follows:

Software/firmware settings for the STM32F103CBT6 microprocessor
Libraries used
Snake game design logic and functions used
Memory/resource consumption of the microcontroller

1) STM32 Microprocessor Software/Firmware Settings

![d13](https://github.com/user-attachments/assets/80f3c1ab-0ee9-4121-a79b-d18fb734b717)


STM32 Microprocessor Settings - New UPDATE
Old Configuration (Left) vs New Configuration (Right)
As can be seen in the image above, in the project, besides the test software settings, I added additional pins for communication with the screen (SPI type) and 4 input pins. Pin A5 represents the pin that creates the clock connection between devices (Serial Clock), pin A7 (Serial Data) creates the data connection between devices, and pins PB6, PB5, and PB4 are the pins for reset, chip select, and register select. The screen uses Half Duplex Master communication, meaning that only the master sends data to the peripheral device, and not vice versa. The classic MISO pin does not exist; I will only use the MOSI pin (namely the SDA pin - Serial Data).
Pins PB12, PB13, PB14, and PB15 are INPUT pins to which I will connect the directional buttons to move the character (snake) on the screen. These will be implemented using a PullDown resistor, so once I press the button, a HIGH signal will be sent to the pin.
I will also use the external quartz clock on the board for much better performance (fast Program Counter), having a value of 72 MHz. For timer usage, I will set a prescaler of 7199, thus obtaining a frequency of 10 kHz. The counter period will have the role of counting clock signals; setting it to a value of 100, I will obtain 10 kHz/100, meaning 100. This number indicates that every second the system interrupt will occur 100 times (thus the interrupt function within the software will be called 100 times per second, allowing me to keep track of the passage of a certain time interval, which is useful in game design).
Also, the already configured pins are RCC_OSC_IN/RCC_OSC_OUT PD0 which synchronize microprocessor operations with an external clock signal, TIM2_CH1 for the timer I set, PA10 and PA9 for UART communication, PA11 and PA12 for USB communication, as well as various pins for an RGB LED and RESET and USER buttons.
Subsequent modification:
Following the software development of the game, it reached a fairly high complexity level, so during testing I observed that there is a considerable delay regarding input reading (there is a delay not due to Debouncing but to code complexity). Therefore, I decided that input should not be handled in the standard way, with a condition being checked in a certain code area, but as a system interrupt. The buttons are now not set as normal inputs but as external system interrupts. Once I press a direction change button (event), a system interrupt will be activated, namely a special function will be called, a function in which I will change the snake's direction of movement.
3) Libraries Used
Library utilized 1 & Library utilized 2
In software development, I used libraries specific to the st7735 screen driver, along with the main Adafruit_GFX graphics library. I added 3 source files (.c) and 3 header files (.h): st7735, GFX_FUNCTIONS, and fonts, in which the operating drivers between the screen and microcontroller are described and set. I find in this library various specific functions such as coloring pixels at certain Cartesian coordinates, coloring the entire screen, or displaying a string (character string) on the screen. Also, in the main header ST7735.h, I set the hardware configuration pins (RST, CS, RS), the type of screen used (resolution), as well as the orientation.
4) Snake Game Design Logic and Functions Used

![d10](https://github.com/user-attachments/assets/6aafb970-555c-4adc-9748-86d66cff42ff)

![d11](https://github.com/user-attachments/assets/d9882394-6671-44fd-aa56-1c49b3d03ee6)

![d12](https://github.com/user-attachments/assets/e579933d-e986-4b59-9543-959681afd821)

Snake Game Design
To design the classic Snake game, I need to follow the spatial/mathematical configuration of the LCD screen. The screen used has a resolution of 128x128; practically I have 128 pixels in both length and width. The coordinate axis (0,0) is the top-left corner of the screen. X and Y increase from top to bottom and from left to right.
To create the snake on the screen, I decided through a simple calculation to divide the screen. If I have 128 pixels, I divided the screen by 8 (length and width), so I can occupy in both directions (x,y) a number of 128/8 = 16 spaces. A number of 16 spaces is sufficient for game implementation; depending on preferences, I could also choose a number of 128/4 = 32 spaces, but I considered 16 would be ideal, especially since the screen used is quite small. Therefore, food and the snake will be represented by an 8x8 pixel square, solving the screen spacing problem.
Additionally, I can choose whether to shrink the game screen to see the score in real-time in a bottom row left free of 8 pixels height, but having a small screen, I preferred to display the score once the game has ended, leaving the player more space.
Snake Game Logic
I need to follow crucial stages in software development of the game such as:
Building the snake as a vector of structures, where I will define a typedef struct Snake structure that contains 2 variables of type uint8_t, x and y, which represent the main position of the snake element. Considering that the maximum length is 128, I use the uint8_t data type because it only occupies 8 bits in memory (1 byte), whereas an int would occupy 4 bytes in memory (4 times more).
Pixel generator function/coordinate assignment: Because I divided the screen into 16x16 squares, I will choose a generator pixel starting from an x,y coordinate whose values are multiples of 8. Practically, I choose a pivot from which to start displaying the snake's head/body in pixels. Thus, for example, to fill the first line using such an algorithm, I start from coordinate (0,0) which is a multiple of 8, color to the right and down 8x8 pixels forming a square, then move on x by 8 to color the next pixel square again, meaning I will be at coordinate (0,8) then at (0,16) up to (0,120). Practically, the generator and reference pivot for each snake body or food represents the pixel at the top-left coordinate of the respective body (this is why it's also called pivot/generator).
Initial spawning of the snake (implementing random functions), which should be between 0 and 120 (Multiple of 8, to respect the spatial configuration and avoid collisions).
Food spawning (also random function) which should first check whether the spawn position is or is not occupied by the snake's body, also with initialization pivot chosen randomly between 0 and 120.
Snake movement logic:
The update_snake() function that goes through each snake structure vector and colors the snake the same color as the background (The user will not perceive this change because the screen refresh rate is too high, the same principle as PWM). Once the pixels have been deleted from the visual sphere, this does not mean the x and y coordinates are also deleted. These will be recalculated based on the user's directional input. The principle behind is the same as mathematical permutations; if I have 1 2 3 4 where 1 is the snake's head, and I permute to the left (user moves left), I will obtain 1 _ 2 3 4, 1 2 _ 3 4, 1 2 3 _ 4, 1 2 3 _, practically if I look at the last iteration of the permutation, I can make the analogy with spatial coordinates, namely the position of the one on the right will take the position of the one on its left, therefore a simple permutation in a for loop can solve the movement problem, and finally I move the snake's head separately because the permutation will stop at it. Depending on the direction of movement, I will move with +8/-8 on x/y according to the logic presented earlier. Once the coordinates have been updated, I can use a draw_snake() function, which takes each snake from the vector and draws it starting from each one's pivot coordinates.
An important observation is that the score will play an important role in establishing the snake's length; it will start from 0 which represents the existence of a single element in the snake, namely its own head snake[0], and with the score update, it should increase by +1 and add therefore in the snake[] vector, the new elements. At each iteration of a main game_logic() function (in which all the physics/dynamics of the game will be found), for example, called in the interrupt function, a delay of 250/500/750 will be added to increase or decrease the snake's movement speed on the screen.
Also, if I move to the right and the direction change input is to the left (or if I move up and the input is down and vice versa), the current movement state will be maintained, without the input having any effect.
Score update logic/length increase:
Every time the snake head's coordinates correspond with those of the food (the x,y pivots of the drawn squares correspond), then the score will increase and the element snake[score].x and snake[score].y will be added, with the food being spawned in another area of the screen, different from that of the snake's body through a simple function.
Game over: At each iteration, if the x or y pivots exceed the boundary coordinates 128 or below 0, then the game logic will stop and the writing "Game Over" will be displayed on the screen along with the obtained score; this will be implemented through a checkCollision() function at each snake movement on the screen.
An important aspect is represented by buttons, which from a software point of view will be implemented taking into account the debouncing phenomenon, to offer the player a rapid response dynamic to input.
Subsequent modification:
Following game development, I added the following modifications:
The defining structure of the snake, which has as internal data the generator pivot coordinates, from the uint8_t data type of these, I changed the coordinates to the int16_t data type. This change is due to the fact that uint8_t is unsigned, so if a value below 0 is assigned to a variable of this data type, I will get buffer overflow which led in the game's physics to "unexpected" errors. An easy alternative was to change from unsigned to signed, but the base of change is in two's complement; the 8-bit variable would take values between 127 and -127, and these variables that designate pixel coordinates can take values up to 128, which again led to the possibility of Buffer Overflow. Therefore, I decided to use uint16_t type variables which are on 16 bits (2 bytes) to no longer encounter this problem.
In more detail, the deletion function goes through the vector of snake coordinate structures and for each element, starting from the generator pivot, I go through the 8x8 pixel section and color it the same color as the background (black in this case) which creates the deletion effect from a visual point of view.
For random food spawning, I used the predefined srand() function. This function uses a starting seed for rng, and if I used the same seed, I would always get the same spawn zones. To give an authentic effect to the game, I thought to use HAL_GetTick as seed, which returns the number of ticks that have passed since system startup. Practically srand(HAL_GetTick) will take the clock tick as seed, and if I declare it within the while(1) function in main, at each iteration of it, the seed will change, obtaining completely random values each time. Also, it is worth noting that I want to obtain values between 0 and 120 as multiples of eight, so once the number has been generated, I will need to process it like this: (random_nr%16)*8 always obtaining a number multiple of 8 from 0 to 15*8.
I also added an initial interface where I can select speed (low/high) using the right and left direction change buttons.
The score is also extremely important in implementation. The snake starts with 3 elements, initially positioned in the middle of the screen, the snake's length being described by the score. Practically snake[score] represents the snake's current length (including element 0). When displaying the game over interface, I will subtract 2 from the obtained score (the snake starts with 0 1 and 2, so I subtract 2 to get the real score). The score variable will therefore be initialized with 2.
The way I add elements to the snake's tail is also important. To efficiently add the new element to its tail, after I verify that the head's pivots with those of the food correspond, I increase the score (score++) and then I make the difference between the last 2 elements from the tail. Thus, I can decide in which direction and according to which coordinates to add the new element. As an example, if the last two elements from the tail (reference pixel coordinates) have the same value on the y axis and the last element on y is 8 less than the second, it means I will need to add the new element (pivot pixel) to the left on the y axis (on the same line) with -8 on the x axis relative to the last element's coordinate.
Also, to increase the game's complexity, I made it possible to pass through walls. The concept is quite simple; it will be verified within the movement functions if the snake's coordinates on the x and y axes have not reached a certain limit (in this case < than 0 or > than 120), in which case the head's new coordinate will be at the opposite limit of the screen.
The buttons have been modified and will be implemented as interrupts; also, the debouncing phenomenon is eliminated through a small delay in the external interrupt function of the buttons which no longer allows direction change for at least 15ms.
Implementation Flowchart
[Flowchart]
5) Microcontroller Memory/Resource Consumption
Resource Consumption
The STM32F103CBT6 microcontroller has the following specifications:

128 KB Flash memory
20 KB RAM memory
72 MHz operating frequency
14 ADC conversion channels
DMA Controller (Direct Memory Access)
USB 2.0 Interface

As can be observed from the image above, the flashed program occupies a size of 0x08007774-0x08000000=0x00007774 which in decimal represents 30580 Bytes, namely approximately 30580/1024 = 29.86 KBytes. The total available flash memory is 128 KB, so I have no problem regarding memory consumption.
I used resources such as timers, SPI communication, external interrupts, and UART communication.
PCB Assembly (Mockup Creation)
Initially, the USB port and microcontroller were soldered onto the board using the Pick and Place mechanism, then placed in the soldering oven, after careful microscope verification (I checked if the alloy was correctly placed on the pads, then if the component pins were properly positioned on the pads).
Image d1
Subsequently, components common to all teams were manually soldered onto the board, using the lab's soldering station.
Image d2
I started preparing the final mockup by soldering the screen onto the board, with connections to the necessary pins being made on the back of the board with the help of wires. Subsequently, I also soldered the 4 buttons (their placement being intuitive to game directions); a button's connection was created diagonally between two of its pins.
Image d3 & Image d4
To be able to make connections between peripheral terminals and microcontroller pins, I soldered each terminal to a male-to-male wire, then did the same for the microcontroller pins.
Image d2
Pin Interconnection
1. GRAPHIC LCD DISPLAY:
The screen's RST (Reset) pin will be connected to a GPIO (General Purpose Input/Output) pin of the STM32F103C8T6, which I will use to reset the screen when necessary. Subsequently, it was connected to PB6 (output).
The RS (Register Select) pin is used to select between data and commands. This should be connected to a GPIO pin of the microcontroller. Subsequently, it was connected to PB4 (output).
The CS (Chip Select) pin is used to select the screen as the active device during SPI communication. This must also be connected to a GPIO pin of the microcontroller. Subsequently, it was connected to PB5 (output).
The 3V3 pin will be connected to the microcontroller's 3v3 power supply pin.
The SDA pin is the data line for SPI communication. This must be connected to the MOSI (Master Out Slave In) data line of the microcontroller. (PA7 - Port A, Pin 7)
The SCL pin ensures synchronization of serial communication for data transmission and reception. It was set to pin PA5_SCK1.
The GND pin must be connected to the microcontroller's ground (GND) pin.
The LED pin has the role of controlling the brightness of the screen's backlight LEDs. (possible implementation: potentiometer for brightness adjustment) - it will be connected to 3.3V power supply.
2. BUTTONS:
I will connect the 4 buttons to different GPIO pins set as read pins (PB12, PB13, PB14, PB15). The buttons have 4 terminals, and once the button is pressed, an internal connection will be made diagonally between pins. (the other 2 terminals used for subsequent hardware verifications).
OBSERVATION: Even though the screen pins induce the thought of an I2C type connection, in fact the screen uses an SPI protocol. SDA (serial data) is actually the analog of the MOSI (master out slave in) pin and, because the screen only receives data, there is no need for a MISO pin, with a special pin existing for separating received commands (between screen setting commands and display commands).
Board and Additional Components Acquired
For programming using the bootloader and communication with the board, I acquired a USB-TTL converter which I connected as follows: pin 1 of J7 on the board to the "5V" pin of the adapter.
Image d3
Electrical Schematic
The electrical schematic presents all components existing on the board, both those provided by the faculty and those specific to the chosen theme:

STM32 microcontroller
Power LED and RGB LED
Buttons for user, reset, and boot
Quartz oscillator for providing a clock signal, necessary for synchronizing the microcontroller's internal operations
Voltage regulator that converts the 5V supply voltage to 3.3V
Type B microUSB port
Jumper for USB-TTL connection
4 buttons used for directions, to which pull-down resistors were added to avoid leaving the input pin "floating" when the button is not pressed
Graphic LCD display

Game Operation
The implemented Snake game operates through interaction between the STM32 microcontroller and the display, utilizing serial SPI communication to transmit display data. The microcontroller continuously updates the LCD's display buffer, drawing game elements such as the snake and food based on the current game state and user inputs. Input from the buttons is read by the microcontroller and interpreted as movement commands for the snake (each button has implemented control logic that assigns a direction: up/down/left/right).
In parallel, the microcontroller performs continuous checks to detect collisions between the snake and the screen edges (comparing the snake head's position with known screen boundaries) or its own body (comparing the head's position with the positions of body segments). To optimize these checks, I store the positions of the head and snake body segments in a list/array. If a collision is detected (compared positions are equal), the game ends and the final score is displayed on screen.
Subsequent modification: Desiring to create a more interactive and complex game, I abandoned the idea of ending the game upon collision with screen edges. Thus, the snake is capable of passing through walls and appearing discontinuously on the opposite side of the screen, continuing its movement normally; the only way the game ends is through the snake colliding with itself.
Food is generated at a random position on the screen, defining a valid zone where it can appear (the entire screen surface minus the space already occupied by the snake's body or previously generated food). I randomly generate two coordinates (x,y) within the defined valid space. With each update of the snake's position, I check the intersection of the head with the food, and in case of a match, its length increases and the score is updated.
The scoring logic is implemented to track the user's performance throughout the game. At the start of the game, I initialize the score to zero. When the snake consumes food, I increase the player's score (each piece of food consumed brings a certain number of points, and I increment the score by a fixed value). I display the score on screen in a corner in real-time, and it appears centrally at the end.
Problems Encountered During Implementation
Throughout software and hardware development, I encountered various problems such as:

The display not functioning in the VSCode working environment.
Finding an efficient method for soldering physical connections while avoiding short circuits.
Implementing an efficient method for adding new snake segments after consuming food.
Generating food position (RNG) and avoiding collision with the snake's body.
Resolving an overflow issue for an unsigned 8-bit variable that decremented below the minimum allowed value (0).
In the loop for verifying food position correctness, I omitted comparing with the snake head's position (I used <0 instead of <=0).

Optimizations Made During Implementation
Regarding optimization, I added external interrupts for inputs, ensuring that pull-down is respected and that interrupts are checked on the rising edge. Additionally, I stylized the game by adding eyes to the snake, making it more attractive. I used vivid colors in the game design to improve users' visual experience. Furthermore, the snake can pass through walls, increasing the complexity level of both the game and software development.
