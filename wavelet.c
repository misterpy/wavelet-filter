#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

//_cache_malloc, _cache_free
#include <tmlib/tmlibc.h>

//custom operations
#include <ops/custom_defs.h>

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

#define W 512
#define H 512

#define W_O 256
#define H_O 256

void read_raw( uint8_t *Inp, int w, int h){
	// int f = open( "J:/Bauernöppel/TestBilder/lena512.raw", O_RDONLY | O_BINARY );

	int f = open( "target-photo/lena512.raw", O_RDONLY | O_BINARY );

	int len = read (f, Inp, w*h);
	if (len != w*h){
		printf("failed to read input, exit\n");
		exit(-1);
	}
	close(f);
}

void write_pgm( const uint8_t *Out,  int w, int h){
	char header[32];
	// int f = open( "I:/CE5/CA2/output/target_lena_a3.pgm", O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0666 );
	int f = open( "output/target_lena_v2.pgm", O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0666 );
	if (f < 0){
		printf("failed to write output, exit\n");
		exit(-1);
	}
	sprintf( header, "P5 %d %d 255\n", w, h );
	write( f, header, strlen(header));
	write( f, Out, w*h );
	close(f);	
}

/*
Funktion mit if und modulo funktion um die unerwartete Werte
auszufiltern.

Aber viele if bedeuetet viele Verzweigerung.

Viele Verzweigerung = nicht optimiert, langsam.
*/
void wavelet_filter_v1(uint8_t *Out, uint8_t *Inp, int w, int h){
	int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 2, i=0; y<h; y++){					//mit Randbehandlung
		if(y % 2 == 0){
			for (x = 2, j=0; x<w; x++){			//mit Randbehandlung (outer Pixels werden beschrieben)
				if(x % 2 == 0){
					
					// 5x5 Umgebung von Inp Pixel laden
					// Single Assignment code -- Code wird in Register zugewiesen
					
					int x00 = *(Inp + W*(y-2) + x-2);
					int x01 = *(Inp + W*(y-2) + x-1);
					int x02 = *(Inp + W*(y-2) + x  );
					int x03 = *(Inp + W*(y-2) + x+1);
					int x04 = *(Inp + W*(y-2) + x+2);
					
					int x10 = *(Inp + W*(y-1) + x-2);
					int x11 = *(Inp + W*(y-1) + x-1);
					int x12 = *(Inp + W*(y-1) + x  );
					int x13 = *(Inp + W*(y-1) + x+1);
					int x14 = *(Inp + W*(y-1) + x+2);

					int x20 = *(Inp + W*(y) + x-2);
					int x21 = *(Inp + W*(y) + x-1);
					int x22 = *(Inp + W*(y) + x  );
					int x23 = *(Inp + W*(y) + x+1);
					int x24 = *(Inp + W*(y) + x+2);
					
					int x30 = *(Inp + W*(y+1) + x-2);
					int x31 = *(Inp + W*(y+1) + x-1);
					int x32 = *(Inp + W*(y+1) + x  );
					int x33 = *(Inp + W*(y+1) + x+1);
					int x34 = *(Inp + W*(y+1) + x+2);
					
					int x40 = *(Inp + W*(y+2) + x-2);
					int x41 = *(Inp + W*(y+2) + x-1);
					int x42 = *(Inp + W*(y+2) + x  );
					int x43 = *(Inp + W*(y+2) + x+1);
					int x44 = *(Inp + W*(y+2) + x+2);
					
					//Wavelet Filter Koeffizienten
					int sum = 1*x00 + (-2*x01) + (-6*x02) + (-2*x03) + 1*x04		//  1 -2 -6 -2  1
					 		+  (-2*x10) + 4*x11 + 12*x12 + 4*x13 + (-2*x14) 		// -2  4 12  4 -2
							+  (-6*x20) + 12*x21 + 36*x22 + 12*x23 + (-6*x24)		// -6 12 36 12 -6
							+  (-2*x30) + 4*x31 + 12*x32 + 4*x33 + (-2*x34)			// -2  4 12  4 -2
							+   1*x40 + (-2*x41) + (-6*x42) + (-2*x43) + 1*x44;		//  1 -2 -6 -2  1
					
					Out[(W/2) * i + j] = sum / 64;
					j++;
				}
				
			}
			i++;
		}
	}
}

/*
Hier ist die Funktion ohne if verzweigerung.
*/
void wavelet_filter_v2(uint8_t *Out, uint8_t *Inp, int w, int h){
	int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 2, i=0; y<h; y+=2){					//mit Randbehandlung
		for (x = 2, j=0; x<w; x+=2){			//mit Randbehandlung (outer Pixels werden beschrieben)
			// 5x5 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
					
			int x00 = *(Inp + W*(y-2) + x-2);
			int x01 = *(Inp + W*(y-2) + x-1);
			int x02 = *(Inp + W*(y-2) + x  );
			int x03 = *(Inp + W*(y-2) + x+1);
			int x04 = *(Inp + W*(y-2) + x+2);
					
			int x10 = *(Inp + W*(y-1) + x-2);
			int x11 = *(Inp + W*(y-1) + x-1);
			int x12 = *(Inp + W*(y-1) + x  );
			int x13 = *(Inp + W*(y-1) + x+1);
			int x14 = *(Inp + W*(y-1) + x+2);

			int x20 = *(Inp + W*(y) + x-2);
			int x21 = *(Inp + W*(y) + x-1);
			int x22 = *(Inp + W*(y) + x  );
			int x23 = *(Inp + W*(y) + x+1);
			int x24 = *(Inp + W*(y) + x+2);

			int x30 = *(Inp + W*(y+1) + x-2);
			int x31 = *(Inp + W*(y+1) + x-1);
			int x32 = *(Inp + W*(y+1) + x  );
			int x33 = *(Inp + W*(y+1) + x+1);
			int x34 = *(Inp + W*(y+1) + x+2);
					
			int x40 = *(Inp + W*(y+2) + x-2);
			int x41 = *(Inp + W*(y+2) + x-1);
			int x42 = *(Inp + W*(y+2) + x  );
			int x43 = *(Inp + W*(y+2) + x+1);
			int x44 = *(Inp + W*(y+2) + x+2);
					
			//Wavelet Filter Koeffizienten
			int sum = 1*x00 + (-2*x01) + (-6*x02) + (-2*x03) + 1*x04		//  1 -2 -6 -2  1
			 		+  (-2*x10) + 4*x11 + 12*x12 + 4*x13 + (-2*x14) 		// -2  4 12  4 -2
					+  (-6*x20) + 12*x21 + 36*x22 + 12*x23 + (-6*x24)		// -6 12 36 12 -6
					+  (-2*x30) + 4*x31 + 12*x32 + 4*x33 + (-2*x34)			// -2  4 12  4 -2
					+   1*x40 + (-2*x41) + (-6*x42) + (-2*x43) + 1*x44;		//  1 -2 -6 -2  1
					
			Out[(W/2) * i + j] = sum / 64;
			j++;
			
		}
		i++;
	}
}

/*
Optimierung mit const. Const heisst dass keine Schreibzugriff *Inp =
erlaubt ist.

Aber meine Schaetzung, dass es wird nicht optimierte, da es sagt nicht,
dass der Speicherbereich *Inpt an sich konstant bleibt.
*/
void wavelet_filter_restrict_const(uint8_t *restrict Out, const uint8_t *Inp, int w, int h){
int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 2, i=0; y<h; y+=2){					//mit Randbehandlung
		for (x = 2, j=0; x<w; x+=2){			//mit Randbehandlung (outer Pixels werden beschrieben)
			// 5x5 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
					
			int x00 = *(Inp + W*(y-2) + x-2);
			int x01 = *(Inp + W*(y-2) + x-1);
			int x02 = *(Inp + W*(y-2) + x  );
			int x03 = *(Inp + W*(y-2) + x+1);
			int x04 = *(Inp + W*(y-2) + x+2);
					
			int x10 = *(Inp + W*(y-1) + x-2);
			int x11 = *(Inp + W*(y-1) + x-1);
			int x12 = *(Inp + W*(y-1) + x  );
			int x13 = *(Inp + W*(y-1) + x+1);
			int x14 = *(Inp + W*(y-1) + x+2);

			int x20 = *(Inp + W*(y) + x-2);
			int x21 = *(Inp + W*(y) + x-1);
			int x22 = *(Inp + W*(y) + x  );
			int x23 = *(Inp + W*(y) + x+1);
			int x24 = *(Inp + W*(y) + x+2);

			int x30 = *(Inp + W*(y+1) + x-2);
			int x31 = *(Inp + W*(y+1) + x-1);
			int x32 = *(Inp + W*(y+1) + x  );
			int x33 = *(Inp + W*(y+1) + x+1);
			int x34 = *(Inp + W*(y+1) + x+2);
					
			int x40 = *(Inp + W*(y+2) + x-2);
			int x41 = *(Inp + W*(y+2) + x-1);
			int x42 = *(Inp + W*(y+2) + x  );
			int x43 = *(Inp + W*(y+2) + x+1);
			int x44 = *(Inp + W*(y+2) + x+2);
					
			//Wavelet Filter Koeffizienten
			int sum = 1*x00 + (-2*x01) + (-6*x02) + (-2*x03) + 1*x04		//  1 -2 -6 -2  1
			 		+  (-2*x10) + 4*x11 + 12*x12 + 4*x13 + (-2*x14) 		// -2  4 12  4 -2
					+  (-6*x20) + 12*x21 + 36*x22 + 12*x23 + (-6*x24)		// -6 12 36 12 -6
					+  (-2*x30) + 4*x31 + 12*x32 + 4*x33 + (-2*x34)			// -2  4 12  4 -2
					+   1*x40 + (-2*x41) + (-6*x42) + (-2*x43) + 1*x44;		//  1 -2 -6 -2  1
					
			Out[W/2 * i + j] = sum / 64;
			j++;
			
		}
		i++;
	}
}

/*
Erklaerung:
Die Benutzung von restrict pointer optimisation ist wie der Programmierer
verspricht, dass auf Array Out nur ueber den Pointer Out zugegriffen wird
und niemals ueber Ponter Inp.
*/
void wavelet_filter_restrict(uint8_t *restrict Out, uint8_t *restrict Inp, int w, int h){
int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 2, i=0; y<h; y+=2){					//mit Randbehandlung
		for (x = 2, j=0; x<w; x+=2){			//mit Randbehandlung (outer Pixels werden beschrieben)
			// 5x5 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
					
			int x00 = *(Inp + W*(y-2) + x-2);
			int x01 = *(Inp + W*(y-2) + x-1);
			int x02 = *(Inp + W*(y-2) + x  );
			int x03 = *(Inp + W*(y-2) + x+1);
			int x04 = *(Inp + W*(y-2) + x+2);
					
			int x10 = *(Inp + W*(y-1) + x-2);
			int x11 = *(Inp + W*(y-1) + x-1);
			int x12 = *(Inp + W*(y-1) + x  );
			int x13 = *(Inp + W*(y-1) + x+1);
			int x14 = *(Inp + W*(y-1) + x+2);

			int x20 = *(Inp + W*(y) + x-2);
			int x21 = *(Inp + W*(y) + x-1);
			int x22 = *(Inp + W*(y) + x  );
			int x23 = *(Inp + W*(y) + x+1);
			int x24 = *(Inp + W*(y) + x+2);

			int x30 = *(Inp + W*(y+1) + x-2);
			int x31 = *(Inp + W*(y+1) + x-1);
			int x32 = *(Inp + W*(y+1) + x  );
			int x33 = *(Inp + W*(y+1) + x+1);
			int x34 = *(Inp + W*(y+1) + x+2);
					
			int x40 = *(Inp + W*(y+2) + x-2);
			int x41 = *(Inp + W*(y+2) + x-1);
			int x42 = *(Inp + W*(y+2) + x  );
			int x43 = *(Inp + W*(y+2) + x+1);
			int x44 = *(Inp + W*(y+2) + x+2);
					
			//Wavelet Filter Koeffizienten
			int sum = 1*x00 + (-2*x01) + (-6*x02) + (-2*x03) + 1*x04		//  1 -2 -6 -2  1
			 		+  (-2*x10) + 4*x11 + 12*x12 + 4*x13 + (-2*x14) 		// -2  4 12  4 -2
					+  (-6*x20) + 12*x21 + 36*x22 + 12*x23 + (-6*x24)		// -6 12 36 12 -6
					+  (-2*x30) + 4*x31 + 12*x32 + 4*x33 + (-2*x34)			// -2  4 12  4 -2
					+   1*x40 + (-2*x41) + (-6*x42) + (-2*x43) + 1*x44;		//  1 -2 -6 -2  1
					
			Out[W/2 * i + j] = sum * 1 / 64;
			j++;
			
		}
		i++;
	}
}


/*
Optimierung mit Type Based Type Analysis.

ANSI-C verlangt, dass auf jeden Speicherbereich waehrend
seiner Lebensdauer nur mit kompatiblen Typen zugegriffen werden darf.

uint32_t und uint8_t sind nicht kompatibel, der Compiler darf
deshalb annehmen, dass y ein separater Speicherbereich ist.
*/
void wavelet_filter_restrict_tbta(uint32_t *Out, uint8_t *Inp, int w, int h){
int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 2, i=0; y<h; y+=2){				//mit Randbehandlung
		for (x = 2, j=0; x<w; x+=2){			//mit Randbehandlung (outer Pixels werden beschrieben)
			// 5x5 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
					
			int x00 = *(Inp + W*(y-2) + x-2);
			int x01 = *(Inp + W*(y-2) + x-1);
			int x02 = *(Inp + W*(y-2) + x  );
			int x03 = *(Inp + W*(y-2) + x+1);
			int x04 = *(Inp + W*(y-2) + x+2);
					
			int x10 = *(Inp + W*(y-1) + x-2);
			int x11 = *(Inp + W*(y-1) + x-1);
			int x12 = *(Inp + W*(y-1) + x  );
			int x13 = *(Inp + W*(y-1) + x+1);
			int x14 = *(Inp + W*(y-1) + x+2);

			int x20 = *(Inp + W*(y) + x-2);
			int x21 = *(Inp + W*(y) + x-1);
			int x22 = *(Inp + W*(y) + x  );
			int x23 = *(Inp + W*(y) + x+1);
			int x24 = *(Inp + W*(y) + x+2);

			int x30 = *(Inp + W*(y+1) + x-2);
			int x31 = *(Inp + W*(y+1) + x-1);
			int x32 = *(Inp + W*(y+1) + x  );
			int x33 = *(Inp + W*(y+1) + x+1);
			int x34 = *(Inp + W*(y+1) + x+2);
					
			int x40 = *(Inp + W*(y+2) + x-2);
			int x41 = *(Inp + W*(y+2) + x-1);
			int x42 = *(Inp + W*(y+2) + x  );
			int x43 = *(Inp + W*(y+2) + x+1);
			int x44 = *(Inp + W*(y+2) + x+2);
					
			//Wavelet Filter Koeffizienten
			int sum = 1*x00 + (-2*x01) + (-6*x02) + (-2*x03) + 1*x04		//  1 -2 -6 -2  1
			 		+  (-2*x10) + 4*x11 + 12*x12 + 4*x13 + (-2*x14) 		// -2  4 12  4 -2
					+  (-6*x20) + 12*x21 + 36*x22 + 12*x23 + (-6*x24)		// -6 12 36 12 -6
					+  (-2*x30) + 4*x31 + 12*x32 + 4*x33 + (-2*x34)			// -2  4 12  4 -2
					+   1*x40 + (-2*x41) + (-6*x42) + (-2*x43) + 1*x44;		//  1 -2 -6 -2  1
					
			Out[W/2 * i + j] = sum * 1 / 64;
			j++;
			
		}
		i++;
	}
}

/*
Hier wird eine Optimierung mit pragma durchgefuehrt.
Die Benutzung von pragma ist um den Code aufzurollen beim kompilieren,
da manche befehle nutzt nicht dieselbe CPU. Das heisst es koennen mehrere
befehle in einem CPU Zyklus durchgefuehrt werden.

Die Optimierte Werte vom Aufrollen muss ausgesuct werden.
Aufrollen Werte Auswahl: 1,2,4,8
*/
void wavelet_filter_restrict_pragma(uint8_t *restrict Out, uint8_t *restrict Inp, int w, int h){
int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 2, i=0; y<h; y+=2){			//mit Randbehandlung

		#pragma TCS_unroll=8				// Optimierte Pragma Werte
		#pragma TCS_unrollexact=1

		for (x = 2, j=0; x<w; x+=2){		//mit Randbehandlung (outer Pixels werden beschrieben)
			// 5x5 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
					
			int x00 = *(Inp + W*(y-2) + x-2);
			int x01 = *(Inp + W*(y-2) + x-1);
			int x02 = *(Inp + W*(y-2) + x  );
			int x03 = *(Inp + W*(y-2) + x+1);
			int x04 = *(Inp + W*(y-2) + x+2);
					
			int x10 = *(Inp + W*(y-1) + x-2);
			int x11 = *(Inp + W*(y-1) + x-1);
			int x12 = *(Inp + W*(y-1) + x  );
			int x13 = *(Inp + W*(y-1) + x+1);
			int x14 = *(Inp + W*(y-1) + x+2);

			int x20 = *(Inp + W*(y) + x-2);
			int x21 = *(Inp + W*(y) + x-1);
			int x22 = *(Inp + W*(y) + x  );
			int x23 = *(Inp + W*(y) + x+1);
			int x24 = *(Inp + W*(y) + x+2);

			int x30 = *(Inp + W*(y+1) + x-2);
			int x31 = *(Inp + W*(y+1) + x-1);
			int x32 = *(Inp + W*(y+1) + x  );
			int x33 = *(Inp + W*(y+1) + x+1);
			int x34 = *(Inp + W*(y+1) + x+2);
					
			int x40 = *(Inp + W*(y+2) + x-2);
			int x41 = *(Inp + W*(y+2) + x-1);
			int x42 = *(Inp + W*(y+2) + x  );
			int x43 = *(Inp + W*(y+2) + x+1);
			int x44 = *(Inp + W*(y+2) + x+2);
					
			//Wavelet Filter Koeffizienten
			int sum = 1*x00 + (-2*x01) + (-6*x02) + (-2*x03) + 1*x04		//  1 -2 -6 -2  1
			 		+  (-2*x10) + 4*x11 + 12*x12 + 4*x13 + (-2*x14) 		// -2  4 12  4 -2
					+  (-6*x20) + 12*x21 + 36*x22 + 12*x23 + (-6*x24)		// -6 12 36 12 -6
					+  (-2*x30) + 4*x31 + 12*x32 + 4*x33 + (-2*x34)			// -2  4 12  4 -2
					+   1*x40 + (-2*x41) + (-6*x42) + (-2*x43) + 1*x44;		//  1 -2 -6 -2  1
					
			Out[W/2 * i + j] = sum / 64;
			j++;
			
		}
		i++;
	}
}


uint8_t Inp[H][W]; //input Bild
uint8_t Out[H_O][W_O]; //output Bild

int main(){

	read_raw( Inp, W, H );
	//wavelet_filter_v1( Out, Inp, W, H);
	//wavelet_filter_v2( Out, Inp, W, H);
	//wavelet_filter_restrict_const(Out, Inp, W, H);
	//wavelet_filter_restrict(Out, Inp, W, H);
	//wavelet_filter_restrict_tbta(Out, Inp, W, H);
	wavelet_filter_restrict_pragma(Out, Inp, W, H);
	write_pgm( Out, W_O, H_O );

	return 0;
	
}
