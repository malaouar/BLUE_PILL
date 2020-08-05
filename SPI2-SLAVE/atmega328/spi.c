
#include <avr/io.h>
#include <util/delay.h>


#define CS_PIN   2     

//==============================
//read and write one byte
char SPI_WriteByte(char val){
	SPDR = val;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

//=========================================
//SPI initialize
void SPI_Init(void){
	DDRB |= _BV(2)|_BV(3)|_BV(5); //  PORTB 2,3,5 outputs

    SPCR =  _BV(SPE)|_BV(MSTR);  // high speed SPI
	SPSR |= _BV(SPI2X);
	
	PORTB |=  _BV(CS_PIN);  // CS=1
}

//========================================
int main(void){

	DDRB |= _BV(0); //  PORTB 0 output for LED
	PORTB ^= (1 << 0);//togle LED
	
    SPI_Init();
		
	PORTB &= ~_BV(CS_PIN)  ; // CS=0
	
	SPI_WriteByte('L');
	_delay_ms(1000);
	PORTB ^= (1 << 0);  //togle LED
	SPI_WriteByte('A');
	_delay_ms(1000);
	PORTB ^= (1 << 0);//togle LED
	SPI_WriteByte('O');
	_delay_ms(1000);
	PORTB ^= (1 << 0);//togle LED
	SPI_WriteByte('U');
	_delay_ms(1000);
	PORTB ^= (1 << 0);//togle LED
	SPI_WriteByte('A');
	_delay_ms(1000);
	PORTB ^= (1 << 0);//togle LED
	SPI_WriteByte('R');
	_delay_ms(1000);
	PORTB ^= (1 << 0);//togle LED	

	while(1){
		SPI_WriteByte('M');
		_delay_ms(1000);
		PORTB ^= (1 << 0);//togle LED
	}
	
	PORTB |=  _BV(CS_PIN);     //CS=1
	
	return 0;
}

	
	
