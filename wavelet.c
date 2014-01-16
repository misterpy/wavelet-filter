#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

typedef unsigned char uint8_t;

#define W 512
#define H 512

void read_raw(uint8_t Inp[H][W], int w, int h)
{
	//int f = open( "J:/Bauernöppel/TestBilder/lena512.raw", O_RDONLY | O_BINARY );
	int f = open( "J:/Bauernöppel/TestBilder/TARGET.raw", O_RDONLY | O_BINARY );
	int len = read( f, Inp, w*h );
	if( len != w*h )
	{
		printf("failed to read input, exit\n");
		exit(-1);
	}
	close(f);
}

void write_pgm(uint8_t Out[H][W], int w, int h)
{
	char header[32];
	// Pfad muss schreibbar sein
	int f = open( "J:/Bauernöppel/CA2/GaussFilter/output.pgm", O_WRONLY|O_CREAT|O_TRUNC|O_BINARY );
	if( f < 0 )
	{
		printf("failed to write output, exit\n");
		exit(-1);
	}
	sprintf( header, "P5 %d %d 255\n", w, h );
	write( f, header, strlen(header) );
	write( f, Out, w*h );
	close(f);	
}

void process_2D( uint8_t Out[][W], uint8_t Inp[][W], int w, int h)
{
	int x, y;

	// berechne Out aus Inp	
	for( y=0; y<h; y++ )
	{
		for( x=0; x<w; x++)
		{
			Out[y][x] = 255 - Inp[y][x];
		}
	}
}

void process_2Da( uint8_t *Out, uint8_t *Inp, int w, int h)
{
	int x, y;

	// berechne Out aus Inp	
	for( y=0; y<h; y++ )
	{
		for( x=0; x<w; x++)
		{
			//Out[y][x] = 255 - Inp[y][x];
			// ist daselbe wie
			*(Out + W*y + x) = 255 - *(Inp + W*y + x);
			
			// ist daselbe wie
			//Out[W*y + x] = 255 - Inp[W*y + x];
		}
	}
}

void gauss_2Da( uint8_t *Out, uint8_t *Inp, int w, int h)
{
	int x, y;

	// berechne Out aus Inp	
	for( y=1; y<h-1; y++ )	// ohne Randbehandlung
	{
		for( x=1; x<w-1; x++)	// ohne Randbehandlung
		{
			// 3x3 Umgebung von Inp Pixel laden
			int x00 = *(Inp + W*(y-1) + x-1 );
			int x01 = *(Inp + W*(y-1) + x   );
			int x02 = *(Inp + W*(y-1) + x+1 );

			int x10 = *(Inp + W*(y  ) + x-1 );
			int x11 = *(Inp + W*(y  ) + x   );
			int x12 = *(Inp + W*(y  ) + x+1 );

			int x20 = *(Inp + W*(y+1) + x-1 );
			int x21 = *(Inp + W*(y+1) + x   );
			int x22 = *(Inp + W*(y+1) + x+1 );

			// 3x3 Filterkoeffizienten 
			// http://de.wikipedia.org/wiki/Gau%C3%9F-Filter
			int sum = 1*x00 + 2*x01 + 1*x02
			        + 2*x10 + 4*x11 + 2*x12
			        + 1*x20 + 2*x21 + 1*x22;
			          
			Out[W*y + x] = sum / 16;
		}
	}
}

uint8_t Inp[H][W]; // input Bild 
uint8_t Out[H][W]; // output Bild 

int main()
{
	read_raw( Inp, W, H );	
	gauss_2Da( Out, Inp, W, H );
	write_pgm( Out, W, H );
	
	return 0;
}
