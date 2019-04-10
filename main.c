// This program demonstrates how to retrieve button-press information from
// the SNES Controller. It polls the controller 30 times a second, and prints
// out the button presses as a binary-encoded 16-bit integer. 0 means unpressed,
// and 1 means pressed.


// Include files
#include "uart.h"
#include "gpio.h"
#include "systimer.h"
#include "framebuffer.h"
#include "mailbox.h"


// Function prototypes
unsigned short get_SNES();
void init_GPIO9_to_output();
void set_GPIO9();
void clear_GPIO9();
void init_GPIO11_to_output();
void set_GPIO11();
void clear_GPIO11();
void init_GPIO10_to_input();
unsigned int get_GPIO10();


void drawMaze();
void drawMazeAt(int x, int y);
void drawSquare(int x, int y, unsigned int colour);


//Defines
#define MAZEX 16
#define MAZEY 12
#define SIZE 64

#define FALSE 0
#define TRUE 1

#define NUMBUTTONS 6

struct Button {
    int number;
    char* name;
};

struct Point {
    int x;
    int y;
};


void getEntrance();
void getExit();

struct Button createButton(int number, char* name);
struct Point createPoint(int x, int y);

// void printPoint(struct Point *p);

struct Point exitPoint;
struct Point entrancePoint;

const int maze[MAZEY][MAZEX] = {
							{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
							{1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
							{2, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1},
							{1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1},
							{1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
							{1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
							{1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1},
							{1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1},
							{1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3},
							{1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1},
							{1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
							{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};





////////////////////////////////////////////////////////////////////////////////
//
//  Function:       main
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function first initializes GPIO pins 9 and 11 as LATCH
//                  and CLOCK output lines, and GPIO pin 10 as a DATA input
//                  line. It then polls the SNES controller 30 times a second,
//                  and prints out the button presses as an encoded 16-bit
//                  integer (in hexadecimal) whenever a button changes state.
//
////////////////////////////////////////////////////////////////////////////////

void main()
{
    unsigned short data, currentState = 0xFFFF;


    // Set up the UART serial port
    uart_init();

    // Set up GPIO pin #9 for output (LATCH output)
    init_GPIO9_to_output();

    // Set up GPIO pin #11 for output (CLOCK output)
    init_GPIO11_to_output();

    // Set up GPIO pin #10 for input (DATA input)
    init_GPIO10_to_input();

    // Clear the LATCH line (GPIO 9) to low
    clear_GPIO9();

    // Set CLOCK line (GPIO 11) to high
    set_GPIO11();

	initFrameBuffer();

    struct Button buttons[NUMBUTTONS];
    buttons[0] = createButton(3, "Start");
    buttons[1] = createButton(4, "Up");
    buttons[2] = createButton(5, "Down");
    buttons[3] = createButton(6, "Left");
    buttons[4] = createButton(7, "Right");
    buttons[5] = createButton(9, "X");

    // Print out a message to the console
    // uart_puts("SNES Controller Program starting.\n");

	//A variable to track if the game has been started yet
	int gameInProgress = FALSE;

	//Declare a point to hold the entrance state
 	getEntrance();

	//Declare a point to hold the end started
	getExit();

	//Declare a point to hold the place of the character
	struct Point character = createPoint(-1, -1);

	drawMaze();

    // Loop forever, reading from the SNES controller 30 times per second
    while (1) {
    	// Read data from the SNES controller
    	data = get_SNES();

	// Write out data if the state of the controller has changed
	if (data != currentState) {

        // Record the state of the controller
	    currentState = data;

        if(data == 0) {
            continue;
        }



		//Draw over the character with the colour of the maze
		if ((character.x != -1) && (character.y != -1)){
			drawMazeAt(character.x, character.y);
		}

        for(int i = 0; i < NUMBUTTONS; i++) {
            if(((1 << buttons[i].number) & data) != 0) {

                switch(buttons[i].number) {
                    // Start
                    case 3 :
					if(gameInProgress == FALSE){
						character.x = entrancePoint.x;
						character.y = entrancePoint.y;
						// character.x = 2;//Hard code, talke out later
						// character.y = 0;//Hard code, take out later
						gameInProgress = TRUE;
					}
                    break;

                    // Up
                    case 4 :
                    if((maze[character.y - 1][character.x] != 1) && (gameInProgress == TRUE)) {
                        character.y -= 1;
                    break;

                    // Down
                    case 5 :
                    if((maze[character.y + 1][character.x] != 1) && (gameInProgress == TRUE)) {
                        character.y += 1;
                    }
                    break;

                    // Left
                    case 6 :
                    if((maze[character.y][character.x - 1] != 1) && (character.x > 0) && (gameInProgress == TRUE)) {
                        character.x -= 1;
                    }
                    break;

                    // Right
                    case 7 :
                    if((maze[character.y][character.x + 1] != 1) && (gameInProgress == TRUE)) {
                        character.x += 1;
                    }
                    break;

                    // X
                    case 9 :
                    // uart_puts("Acid Bonus \n");
                	break;

                    default :
                    break;
                }
            }
        }


	}

		//Check if the character is in the end state
		if ((character.x == exitPoint.x) && (character.y == exitPoint.y)){
			//The game is over and ready to be restarted
			gameInProgress = FALSE;
			//Draw the character in green
			drawSquare(character.x, character.y, 0x0000FF00);
		}
		//If not at the end state then redraw the character in red where it is
		else if(gameInProgress){
			drawSquare(character.x, character.y, 0x00FF0000);
		}


    	// Delay 1/30th of a second
    	microsecond_delay(33333);
    }
}
}


struct Button createButton(int number, char* name){
    struct Button b;
    b.number = number;
    b.name = name;
    return b;
}



struct Point createPoint(int x, int y){
    struct Point p;
    p.x = x;
    p.y = y;
    return p;
}


// void printPoint(const struct Point *p)
// {
//     // When you are using pointers you need to use -> to access members
//     uart_puts("X = ");
//     uart_puthex(p->x);
//     uart_puts("  Y = ");
//     uart_puthex(p->y);
//     uart_puts("\n");
// }


void drawMaze(){
	for (int i = 0; i < MAZEY; i++){
		for (int j = 0; j < MAZEX; j++){
			drawMazeAt(j,i);
		}
	}
}


void drawMazeAt(int x, int y){
	unsigned int colour;

	switch (maze[y][x]){

	case 0:
	colour = 0x00FFFFFF;//White
	break;

	case 1:
	colour = 0x00000000;//Black
	break;

	case 2:
	colour = 0x00FFFFFF;//White
	break;

	case 3:
	colour = 0x00FFFFFF;//White
	break;

	default:
	colour = 0x00FFFF00;	//Yellow == something wrong
	break;
	}

	drawSquare(x, y, colour);
}


void drawSquare(int x, int y, unsigned int colour){
//void drawSquareToFrameBuffer(int rowStart, int columnStart, int squareSize, unsigned int color)
	drawSquareToFrameBuffer( (y * SIZE), (x * SIZE), SIZE, colour);
}


void getExit(){
	for(int localY = 0; localY< MAZEY; localY++){
		for(int localX = 0; localX < MAZEX; localX++){
			if (maze[localY][localX] == 3){	//3 represents the exit in the array
				exitPoint.x = localX;
				exitPoint.y = localY;
				break;
			}
		}
	}
}


void getEntrance(){
	for(int localY = 0; localY< MAZEY; localY++){
		for(int localX = 0; localX < MAZEX; localX++){
			if (maze[localY][localX] == 2){	//2 represents the entrance in the array
				entrancePoint.x = localX;
				entrancePoint.y = localY;
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       get_SNES
//
//  Arguments:      none
//
//  Returns:        A short integer with the button presses encoded with 16
//                  bits. 1 means pressed, and 0 means unpressed. Bit 0 is
//                  button B, Bit 1 is button Y, etc. up to Bit 11, which is
//                  button R. Bits 12-15 are always 0.
//
//  Description:    This function samples the button presses on the SNES
//                  controller, and returns an encoding of these in a 16-bit
//                  integer. We assume that the CLOCK output is already high,
//                  and set the LATCH output to high for 12 microseconds. This
//                  causes the controller to latch the values of the button
//                  presses into its internal register. We then clock this data
//                  to the CPU over the DATA line in a serial fashion, by
//                  pulsing the CLOCK line low 16 times. We read the data on
//                  the falling edge of the clock. The rising edge of the clock
//                  causes the controller to output the next bit of serial data
//                  to be place on the DATA line. The clock cycle is 12
//                  microseconds long, so the clock is low for 6 microseconds,
//                  and then high for 6 microseconds.
//
////////////////////////////////////////////////////////////////////////////////

unsigned short get_SNES()
{
    int i;
    unsigned short data = 0;
    unsigned int value;


    // Set LATCH to high for 12 microseconds. This causes the controller to
    // latch the values of button presses into its internal register. The
    // first serial bit also becomes available on the DATA line.
    set_GPIO9();
    microsecond_delay(12);
    clear_GPIO9();

    // Output 16 clock pulses, and read 16 bits of serial data
    for (i = 0; i < 16; i++) {
	// Delay 6 microseconds (half a cycle)
	microsecond_delay(6);

	// Clear the CLOCK line (creates a falling edge)
	clear_GPIO11();

	// Read the value on the input DATA line
	value = get_GPIO10();

	// Store the bit read. Note we convert a 0 (which indicates a button
	// press) to a 1 in the returned 16-bit integer. Unpressed buttons
	// will be encoded as a 0.
	if (value == 0) {
	    data |= (0x1 << i);
	}

	// Delay 6 microseconds (half a cycle)
	microsecond_delay(6);

	// Set the CLOCK to 1 (creates a rising edge). This causes the
	// controller to output the next bit, which we read half a
	// cycle later.
	set_GPIO11();
    }

    // Return the encoded data
    return data;
}


////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO9_to_output
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 9 to an output pin without
//                  any pull-up or pull-down resistors.
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO9_to_output()
{
    register unsigned int r;


    // Get the current contents of the GPIO Function Select Register 0
    r = *GPFSEL0;

    // Clear bits 27 - 29. This is the field FSEL9, which maps to GPIO pin 9.
    // We clear the bits by ANDing with a 000 bit pattern in the field.
    r &= ~(0x7 << 27);

    // Set the field FSEL9 to 001, which sets pin 9 to an output pin.
    // We do so by ORing the bit pattern 001 into the field.
    r |= (0x1 << 27);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 0
    *GPFSEL0 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 9. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
    // internal pull-up and pull-down resistor isn't needed for an output pin.

    // Disable pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time
    // for the control signal
    r = 150;
    while (r--) {
	asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 9 to
    // clock in the control signal for GPIO pin 9. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 9);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       set_GPIO9
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets the GPIO output pin 9
//                  to a 1 (high) level.
//
////////////////////////////////////////////////////////////////////////////////

void set_GPIO9()
{
    register unsigned int r;

    // Put a 1 into the SET9 field of the GPIO Pin Output Set Register 0
    r = (0x1 << 9);
    *GPSET0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       clear_GPIO9
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function clears the GPIO output pin 9
//                  to a 0 (low) level.
//
////////////////////////////////////////////////////////////////////////////////

void clear_GPIO9()
{
    register unsigned int r;

    // Put a 1 into the CLR9 field of the GPIO Pin Output Clear Register 0
    r = (0x1 << 9);
    *GPCLR0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO11_to_output
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 11 to an output pin without
//                  any pull-up or pull-down resistors.
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO11_to_output()
{
    register unsigned int r;


    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL1;

    // Clear bits 3 - 5. This is the field FSEL11, which maps to GPIO pin 11.
    // We clear the bits by ANDing with a 000 bit pattern in the field.
    r &= ~(0x7 << 3);

    // Set the field FSEL11 to 001, which sets pin 9 to an output pin.
    // We do so by ORing the bit pattern 001 into the field.
    r |= (0x1 << 3);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 1
    *GPFSEL1 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 11. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. The
    // internal pull-up and pull-down resistor isn't needed for an output pin.

    // Disable pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time
    // for the control signal
    r = 150;
    while (r--) {
	asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 11 to
    // clock in the control signal for GPIO pin 11. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 11);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       set_GPIO11
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets the GPIO output pin 11
//                  to a 1 (high) level.
//
////////////////////////////////////////////////////////////////////////////////

void set_GPIO11()
{
    register unsigned int r;

    // Put a 1 into the SET11 field of the GPIO Pin Output Set Register 0
    r = (0x1 << 11);
    *GPSET0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       clear_GPIO11
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function clears the GPIO output pin 11
//                  to a 0 (low) level.
//
////////////////////////////////////////////////////////////////////////////////

void clear_GPIO11()
{
    register unsigned int r;

    // Put a 1 into the CLR11 field of the GPIO Pin Output Clear Register 0
    r = (0x1 << 11);
    *GPCLR0 = r;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       init_GPIO10_to_input
//
//  Arguments:      none
//
//  Returns:        void
//
//  Description:    This function sets GPIO pin 10 to an input pin without
//                  any internal pull-up or pull-down resistors. Note that
//                  a pull-down (or pull-up) resistor must be used externally
//                  on the bread board circuit connected to the pin. Be sure
//                  that the pin high level is 3.3V (definitely NOT 5V).
//
////////////////////////////////////////////////////////////////////////////////

void init_GPIO10_to_input()
{
    register unsigned int r;


    // Get the current contents of the GPIO Function Select Register 1
    r = *GPFSEL1;

    // Clear bits 0 - 2. This is the field FSEL10, which maps to GPIO pin 10.
    // We clear the bits by ANDing with a 000 bit pattern in the field. This
    // sets the pin to be an input pin.
    r &= ~(0x7 << 0);

    // Write the modified bit pattern back to the
    // GPIO Function Select Register 1
    *GPFSEL1 = r;

    // Disable the pull-up/pull-down control line for GPIO pin 10. We follow the
    // procedure outlined on page 101 of the BCM2837 ARM Peripherals manual. We
    // will pull down the pin using an external resistor connected to ground.

    // Disable internal pull-up/pull-down by setting bits 0:1
    // to 00 in the GPIO Pull-Up/Down Register
    *GPPUD = 0x0;

    // Wait 150 cycles to provide the required set-up time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Write to the GPIO Pull-Up/Down Clock Register 0, using a 1 on bit 10 to
    // clock in the control signal for GPIO pin 10. Note that all other pins
    // will retain their previous state.
    *GPPUDCLK0 = (0x1 << 10);

    // Wait 150 cycles to provide the required hold time
    // for the control signal
    r = 150;
    while (r--) {
        asm volatile("nop");
    }

    // Clear all bits in the GPIO Pull-Up/Down Clock Register 0
    // in order to remove the clock
    *GPPUDCLK0 = 0;
}



////////////////////////////////////////////////////////////////////////////////
//
//  Function:       get_GPIO10
//
//  Arguments:      none
//
//  Returns:        1 if the pin level is high, and 0 if the pin level is low.
//
//  Description:    This function gets the current value of pin 10.
//
////////////////////////////////////////////////////////////////////////////////

unsigned int get_GPIO10()
{
    register unsigned int r;


    // Get the current contents of the GPIO Pin Level Register 0
    r = *GPLEV0;

    // Isolate pin 10, and return its value (a 0 if low, or a 1 if high)
    return ((r >> 10) & 0x1);
}
