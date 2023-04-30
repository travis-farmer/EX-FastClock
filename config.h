// Configuration file for EX-Fast_Clock
// Colin H Murdoch - February 2023
// This file comtains basic setting that comtrol how the FastClock operates.

// If we are sending data to the DCC-EX CS via I2C then we need to define an I2C Address
// and set a flag for inclusion. If not using I2C then comment out next two lines

//#define I2CAddress 0x55 // default I2C address
//#define SEND_VIA_I2C true

// If we are sending the data to the CS via Serial then uncomment the following line
#define SEND_VIA_SERIAL true


// Set this following string to the header you require.  This is displayed at the top of the screen
#define HDDR_SIZE  25
char header[HDDR_SIZE] = {"Nantyderry Junction"};


