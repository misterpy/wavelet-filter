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

	int f = open( "target-foto/lena512.raw", O_RDONLY | O_BINARY );

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
	int f = open( "output/target_lena_a3.pgm", O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, 0666 );
	if (f < 0){
		printf("failed to write output, exit\n");
		exit(-1);
	}
	sprintf( header, "P5 %d %d 255\n", w, h );
	write( f, header, strlen(header));
	write( f, Out, w*h );
	close(f);	
}


//void process_2D( uint8_t Out[][W], uint8_t Inp[][W], int w, int h){
//	int x, y;
//	
//	//berechne O aus I
//	for (y = 0; y<h; y++){
//		for (x = 0; x<w; x++){
//			//ist das selbe wie obige Funktion - jedoch 1D
//			//(Out + W * y + x) = 255 - *(Inp + W * y + x);
//			//ist das selbe wie
//			Out[y][x] = 255 - Inp[y][x];
//			//Out[w * y + x] = 255 - Inp[w * y + x];
//		}
//	}
//}

void wavelet_filter_a( uint8_t *Out_temp, uint8_t *Out, uint8_t *Inp, int w, int h){
	int x,y,i,j;
	
	//berechne O_temp aus I
	for (y = 0; y<h; y++){					//mit Randbehandlung
		for (x = 0; x<w; x++){				//mit Randbehandlung (outer Pixels werden nicht beschrieben)
			
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
			
			Out_temp[W * y + x] = sum * 1 / 64;
		}
	}
	

	// Auslesen "Out_temp" zu einem kleineren array als end Output array "Out".
	for (y = 0, i = 0; y<h; y++){
		if(y % 2 == 0){
			for (x = 0, j = 0; x<w; x++){
				if(x % 2 == 0){
					Out[i][j] = Out_temp[y][x];
					j++;
				}
				i++;
			}
		}
	}
}

void wavelet_filter_b( uint8_t *restrict Out, uint8_t *restrict Inp, int w, int h){ //restrict pointer Optimization
	int x, y;
	
	//berechne O aus I
	for (y = 1; y<h-1; y++){					//ohne Randbehandlung

	//Info C:\Trimedia\NDK57\NDK5.7\docs\TCS_Docs\03_CompilationTools.pdf
	#pragma TCS_unroll=4							// Pragma optimized, Schleife viermal aufrollen
	#pragma TCS_unrollexact=1						// folgende Schleife wird ein Vielfaches von 4 mal aufgerufen
		
		for (x = 1; x<w-1; x++){				//ohne Randbehandlung (outer Pixels werden nicht beschrieben)
			
			// 3x3 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
			
			int x00 = *(Inp + W*(y-1) + x-1);	// hier y-1 da die Umgebung um ein Pixel herum verwendet wird
			int x01 = *(Inp + W*(y-1) + x  );	// deshalb auch die Pixel darueber
			int x02 = *(Inp + W*(y-1) + x+1);

			int x10 = *(Inp + W*(y) + x-1);
			int x11 = *(Inp + W*(y) + x  );
			int x12 = *(Inp + W*(y) + x+1);
			
			int x20 = *(Inp + W*(y+1) + x-1);
			int x21 = *(Inp + W*(y+1) + x  );
			int x22 = *(Inp + W*(y+1) + x+1);
			
			//Gauss Filter Koeffizienten
			int sum = 1*x00 + 2*x01 + 1*x02		// 1 2 1
			 		+ 2*x10 + 4*x11 + 2*x12 	// 2 4 2
					+ 1*x20 + 2*x21 + 1*x22;	// 1 2 1
			
			Out[W * y + x] = sum / 16;
		}
	}
}

void wavelet_filter_c( uint8_t *restrict Out, const uint8_t * Inp, int w, int h){ //restrict pointer Optimization
	int x, y;
	
	//berechne O aus I
	for (y = 1; y<h-1; y++){					//ohne Randbehandlung		
		
		for (x = 1; x<w-1; x+=4){				//ohne Randbehandlung (outer Pixels werden nicht beschrieben)
			
			// 3x3 Umgebung von Inp Pixel laden
			// Single Assignment code -- Code wird in Register zugewiesen
			
			int x00 = *(Inp + W*(y-1) + x-1);	// hier y-1 da die Umgebung um ein Pixel herum verwendet wird
			int x01 = *(Inp + W*(y-1) + x  );	// deshalb auch die Pixel darueber
			int x02 = *(Inp + W*(y-1) + x+1);
			int x03 = *(Inp + W*(y-1) + x+2);
			int x04 = *(Inp + W*(y-1) + x+3);
			int x05 = *(Inp + W*(y-1) + x+4);

			int x10 = *(Inp + W*(y) + x-1);
			int x11 = *(Inp + W*(y) + x  );
			int x12 = *(Inp + W*(y) + x+1);
			int x13 = *(Inp + W*(y) + x+2);
			int x14 = *(Inp + W*(y) + x+3);
			int x15 = *(Inp + W*(y) + x+4);
			
			int x20 = *(Inp + W*(y+1) + x-1);
			int x21 = *(Inp + W*(y+1) + x  );
			int x22 = *(Inp + W*(y+1) + x+1);
			int x23 = *(Inp + W*(y+1) + x+2);
			int x24 = *(Inp + W*(y+1) + x+3);
			int x25 = *(Inp + W*(y+1) + x+4);
			
			//Gauss Filter Koeffizienten
			int sum0 = 1*x00 + 2*x01 + 1*x02 + 2*x10 + 4*x11 + 2*x12 + 1*x20 + 2*x21 + 1*x22;
			int sum1 = 1*x01 + 2*x02 + 1*x03 + 2*x11 + 4*x12 + 2*x13 + 1*x21 + 2*x22 + 1*x23;
			int sum2 = 1*x02 + 2*x03 + 1*x04 + 2*x12 + 4*x13 + 2*x14 + 1*x22 + 2*x23 + 1*x24;
			int sum3 = 1*x03 + 2*x04 + 1*x05 + 2*x13 + 4*x14 + 2*x15 + 1*x23 + 2*x24 + 1*x25; 
			
			int out0 = sum0 / 16;
			int out1 = sum1 / 16;
			int out2 = sum2 / 16;
			int out3 = sum3 / 16;
			

			//see C:\Trimedia\NDK57\NDK5.7\docs\Hardware_Reference\TM3260_Arch_Manual_v102.pdf
			uint32_t out0out1 = PACKBYTES( out1, out0);
			uint32_t out2out3 = PACKBYTES( out3, out2);
			uint32_t out0out1out2out3 = PACK16LSB( out0out1, out2out3);
			
			*((uint32_t*)&(Out[W*y + x])) = out0out1out2out3;
			
		}
	}
}

//uint8_t Inp[H][W]; input Bild
//uint8_t Out[H][W]; output Bild

uint8_t *Inp; //input Bild
uint8_t *Out; //output Bild

int main(){

	Inp = _cache_malloc( W*H, -1);
	Out_temp = _cache_malloc( W*H, -1);
	Out = _cache_malloc(W_O*H_O, -1);
	
	read_raw( Inp, W, H );
	wavelet_filter_a( Out_temp, Out, Inp, W, H);
	write_pgm( Out, W/2, H/2 );

	//process_2D( Out, Inp, W, H);
	//wavelet_filter_a( Out, Inp, W, H);
	//wavelet_filter_c( Out, Inp, W, H);
	
	_cache_free(Inp);
	_cache_free(Out);
	_cache_free(Out_temp);

	
	return 0;
	
}
