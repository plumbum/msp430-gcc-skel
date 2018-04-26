// Modified by Ivan Alf <i@tuxotronic.org>
// from example
// Sergio Campama 2011
// http://kaipi.me
//
// Made for the TI MSP430 LaunchPad
// http://ti.com/launchpadwiki
//
// Released under the Beerware License
// http://en.wikipedia.org/wiki/Beerware

// #include <msp430.h>
#include <msp430g2231.h>
#include <inttypes.h>

//In uniarch there is no more signal.h to sugar coat the interrupts definition, so we do it here
#define interrupt(x) void __attribute__((interrupt (x))) 

//Defines to help understand what is attached to each port
#define LED_DIR P1DIR
#define LED_OUT P1OUT

//P1.0 and P1.6 are the RED and GREEN LEDs respectively
#define LED_RED BIT0
#define LED_GREEN BIT6

void led_init(void)
{
	//Set the direction as output
	LED_DIR |= LED_RED; 
	LED_DIR |= LED_GREEN;
	
	LED_OUT |= LED_GREEN;
	LED_OUT &= ~LED_RED;
}

void timer_init(void)
{
	//Select SMCLK, TimerA counter divisor in 1
	TACTL |= TASSEL_2 | ID_0; 

	//Enables the CCR0 interrupt
	TACCTL0 |= CCIE; 
}

void init(void)
{
	//Stop the watchdog
	WDTCTL = WDTPW + WDTHOLD; 

	BCSCTL3 =
		LFXT1S_2 // Use VLOCLK 12 KHz
	;
	BCSCTL2 =
		SELS // Select SMCLK as VLOCLK
	;

	//Inits the different parts
	led_init();
	timer_init();

	//Enable global interrupts
	_BIS_SR(GIE);
}

void timer_start(void)
{
	TACTL |= MC_1; //Sets the TimerA count mode in UP, and thus starting the Timer

	//Since the crystal attached to LFXT is 32.768 kHz, 
	//when the counter reaches this value, exactly 1 second should have passed
	//The clock skew is 20 ppm, which at this frequency, should be +-.65 Hz
	// TACCR0 = 32768;
	TACCR0 = 3000;
}


int main(void)
{
	init();
	timer_start();
	
	//We enter the LPM3 Low Power Mode, which only leaves ACLK running
	//This way, the CPU will be off for 1 second, toggle the LED, and turn off for another second
	_BIS_SR(LPM1_bits);
	
	for(;;);

	return 0;
}

//CCR0 timer interrupt, which toggles the LEDs
#ifndef TIMERA0_VECTOR
#define TIMERA0_VECTOR TIMER0_A0_VECTOR
#endif
interrupt(TIMERA0_VECTOR) TIMERA0_ISR(void) {
	LED_OUT ^= LED_GREEN + LED_RED;
}
