#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

#define BYTE 2
#define WORD 6
#define MAX_ADDRESS 10000
char pomnilnik[MAX_ADDRESS];

// REGISTRI
int32_t A = 0;
int32_t X = 0;
int32_t L = 0;
int32_t S = 0;
int32_t T = 0;
int32_t B = 0;
int32_t SW = 0;			// jednaki(0), prvi je manji(-1), prvi je veci(1)
float F = 0;
int32_t PC = 0;

int ukaz = 0;

int takojsnje = 0;

int END = 0;

int RD[255];
int WD[255];

int32_t hexToDec(char znak){
	if(znak>=48 && znak<=57){
		return znak - 48;
	}
	return znak - 55;
} 

int stepen(int broj, int step){
	if(step == 0){
		return 1;
	}
	int rez = 1;
	for(int i=0; i<step; i++){
		rez = rez * broj;
	}
	return rez;

}

int getBit(int32_t vrednost, int pozicija){   // <----------- Nije testirano
	int mask =  1 << pozicija;
	return (vrednost & mask) >> pozicija;
}

void print(char* pomnilnik, int dolzina){
	for(int i=0; i<dolzina; i++){
		printf("%c", pomnilnik[i]);
	}
	printf("\n");
}

//funkcija za ucitavanje iz fajla v pomnilnk
int ucitaj(FILE *fptr){

	char tre;
	int index = 0;
	
	while((tre = fgetc(fptr)) != '\n') {
  		pomnilnik[index] = tre;
  		index++;
	}
	for(int i=index; i<MAX_ADDRESS; i++){
		pomnilnik[index] = '0';
		index++;
	}
	
	
	return index;
}

char* readFromFile(FILE *fptr, int vrstica){

	char tre;
	int index = 0;
	char* niz = calloc(50, sizeof(char));
	
	int d = RD[vrstica]+1;
	RD[vrstica]++;
	while(d && index<6) {
	
		if((tre = fgetc(fptr)) == '\n'){
			d--;
		}
		if(d==1 && tre != '\n'){
  			niz[index] = tre;
  			index++;
  		}
	}
	return niz;
}

int* getRegister(int a, int b, int c, int d){ // pomocu a, b bita dobijemo odgovarajuci register
	if(a==0){
		if(b==0){
			if(c==0){
				if(d==0){
					return &A;
				}
				else if(d==1){
					return &X;
				}
			}
			else if(c==1){
				if(d==0){
					return &L;
				}
				else if(d==1){
					return &B;
				}
			}
		}
		else if(b==1){
			if(c==0){
				if(d==0){
					return &S;
				}	
				else if(d==1){
					return &T;
				}
			}
			else if(c==1){
				if(d==0){
					//return &F;
				}
			}
		}
	}
	else if(a==1){
		if(d==0){
			return &PC;
		}
		else if(d==1){
			return &SW;
		}
	}
}

int naslavljanje(int b, int p){
	int vrednost = b*2 + p*1;
	if(vrednost == 0){ // neposredno
		return 0;
	}
	else if(vrednost == 1){ // PC-relativno
		return 1;
	}
	else if(vrednost == 2){ // bazno-relativno
		return 2;
	}
}

int KakoVporabitiOperand(int n, int i){
	int vrednost = n*2 + i*1;
	
	takojsnje = 0;
	if(vrednost == 1){ // takojsnje
		takojsnje = 1;
		return 0;
	}
	else if(vrednost == 2){ // posredno
		return 1;
	}
	else if(vrednost == 3){ // enostavno
		return 2;
	}
	else if(vrednost == 0){ // StariSic
		return 3;
	}
}

int getOdmik(int nas, int x, int vrednost){
	
	if(x == 1){
		vrednost += X; 
	}
	if(nas == 0){ // neposredno
		vrednost *= 2;
		return vrednost;
	}
	else if(nas == 1){ // PC-relativno
	
		
		vrednost *= 2;
		vrednost += PC;
	
		
		//printf("PC = %d\n", PC);
		//printf("vrednost = %d\n", vrednost);
		
		
		return vrednost;
	}
	else if(nas == 2){ // bazno-relativno
		
		vrednost *= 2; // PROVERI
		
		vrednost += B;
		
		return vrednost;
	}
}

int dodeli(int kako, int UN, int b, int p, int e, int dolzina, int ST){	

	//print(&pomnilnik[6365], 25);

	if(kako == 0){ // takojsnje
		return UN;
	}
	else if(kako == 1){ // posredno
		
		char* adresa = &pomnilnik[UN];
		int32_t znak3 = hexToDec(adresa[0]); 	// <- za WORD 3 byte
		int32_t znak4 = hexToDec(adresa[1]);
		int32_t znak5 = hexToDec(adresa[2]);
		
		int32_t znak6 = hexToDec(adresa[3]); 
		int32_t znak7 = hexToDec(adresa[4]);
		int32_t znak8 = hexToDec(adresa[5]);
		
		int v =znak3*(1 << 20) + znak4*(1 << 16) + znak5*(1 << 12)  + znak6*(1 << 8) + znak7*(1 << 4) + znak8;	
		
		v *= 2; //---------------------------------------------------------------------------------------------------------------------	
		
		
		if(dolzina == WORD){
			char* adresa2 = &pomnilnik[v];
			
			znak3 = hexToDec(adresa2[0]); 	// <- za WORD 3 byte
			znak4 = hexToDec(adresa2[1]);
			znak5 = hexToDec(adresa2[2]);
			
			znak6 = hexToDec(adresa2[3]); 
			znak7 = hexToDec(adresa2[4]);
			znak8 = hexToDec(adresa2[5]);
			
			//printf("alo = %d\n",znak8);
			
			int g = znak3*(1 << 20) + znak4*(1 << 16) + znak5*(1 << 12)  + znak6*(1 << 8) + znak7*(1 << 4) + znak8;
			if(ST == 1){
				return v;
			}	
			
			//print(adresa2, 6);
			//printf("1 = %c, 2 = %c, 3 = %c, 4 = %c, 5 = %d, 6 = %d,\n", adresa2[0], adresa2[1], adresa2[2], adresa2[3], adresa2[4], adresa2[5]);
			//printf("1 = %d, 2 = %d, 3 = %d, 4 = %d, 5 = %d, 6 = %d,\n", znak3, znak4, znak5, znak6, znak7, znak8);
			
			//printf("v = %d\n",v);
			
			//printf("g = %d\n",g);
				
			return g;
		}
		char* adresa2 = &pomnilnik[v];
		znak3 = hexToDec(adresa2[0]); 	// <- za WORD 3 byte
		znak4 = hexToDec(adresa2[1]);
		int g = znak3*(1 << 4) + znak4;
		if(ST == 1){
			return v;
		}
		return g;
		
	}
	else if(kako == 2){ // enostavno
	
		if(dolzina == WORD){
			char* adresa = &pomnilnik[UN];
			int32_t znak3 = hexToDec(adresa[0]); 	// <- za WORD 3 byte
			int32_t znak4 = hexToDec(adresa[1]);
			int32_t znak5 = hexToDec(adresa[2]);
			
			int32_t znak6 = hexToDec(adresa[3]); 
			int32_t znak7 = hexToDec(adresa[4]);
			int32_t znak8 = hexToDec(adresa[5]);
			
			int v =znak3*(1 << 20) + znak4*(1 << 16) + znak5*(1 << 12)  + znak6*(1 << 8) + znak7*(1 << 4) + znak8;
			
			//printf("UN = %d\n", UN);
			if(ST == 1){
				return UN;
			}
					
			return v;
		}
		char* adresa = &pomnilnik[UN];
		int32_t znak3 = hexToDec(adresa[0]); 	// <- za WORD 3 byte
		int32_t znak4 = hexToDec(adresa[1]);
		int v = znak3*(1 << 4) + znak4;
		if(ST == 1){
			return UN;
		}	
		return v;
		
	}
	else if(kako == 3){ // StariSic
		UN = UN + b*16384 + p*8192 + e*4096;
		
		if(dolzina == WORD){
			char* adresa = &pomnilnik[UN];
			int32_t znak3 = hexToDec(adresa[0]); 	// <- za WORD 3 byte
			int32_t znak4 = hexToDec(adresa[1]);
			int32_t znak5 = hexToDec(adresa[2]);
			
			int32_t znak6 = hexToDec(adresa[3]); 
			int32_t znak7 = hexToDec(adresa[4]);
			int32_t znak8 = hexToDec(adresa[5]);
			
			int v =znak3*(1 << 20) + znak4*(1 << 16) + znak5*(1 << 12)  + znak6*(1 << 8) + znak7*(1 << 4) + znak8;	
			if(ST == 1){
				return UN;
			}
			return v;
		}
		char* adresa = &pomnilnik[UN];
		int32_t znak3 = hexToDec(adresa[0]); 	// <- za WORD 3 byte
		int32_t znak4 = hexToDec(adresa[1]);
		int v = znak3*(1 << 4) + znak4;
		if(ST == 1){
				return UN;
		}	
		return v;
	}
}

int ukazi3in4(char* pomnilnik, int e1 ,int z1, int dolzina, int ST){

		//printf("L = %d\n", L);

		PC += 4;
		int n = e1;
		int i = z1;
		int32_t znak2 = hexToDec(pomnilnik[2]);
		if(ukaz == 1){
			printf("%c", pomnilnik[2]);
		}
		int x = getBit(znak2, 3);
		int b = getBit(znak2, 2);
		int p = getBit(znak2, 1);
		int e = getBit(znak2, 0);
		int kako = KakoVporabitiOperand(n,i);
		int nas = naslavljanje(b,p);
		int vrednost;
		if(e == 0){
			int32_t znak3 = hexToDec(pomnilnik[3]);
			int32_t znak4 = hexToDec(pomnilnik[4]);
			int32_t znak5 = hexToDec(pomnilnik[5]);
			vrednost = znak3*(1 << 8) + znak4*(1 << 4) + znak5;	
			
			if(ukaz == 1){
				printf("%c%c%c\n", pomnilnik[3], pomnilnik[4], pomnilnik[5]);
			}
			
			int d = (vrednost & (0x000F00)) >> 8;
			//printf("d = %d\n", d);
			if(d == 15){ // Negativna vrednost
				vrednost -= 4096;
				if(vrednost == -3){
					END = 1;
				}
			}
		}
		else if(e == 1){
			PC += 2;
			int32_t znak3 = hexToDec(pomnilnik[3]);
			int32_t znak4 = hexToDec(pomnilnik[4]);
			int32_t znak5 = hexToDec(pomnilnik[5]);
			int32_t znak6 = hexToDec(pomnilnik[6]);
			int32_t znak7 = hexToDec(pomnilnik[7]);
			vrednost = znak3*(1 << 16) + znak4*(1 << 12) + znak5*(1 << 8) + znak6*(1 << 4) + znak7;	
			
			//printf("vrednost = %d, %d, %d, %d\n", znak2, znak5, znak6, znak7);
			
			if(ukaz == 1){
				printf("%c%c%c%c%c\n", pomnilnik[3], pomnilnik[4], pomnilnik[5], pomnilnik[6], pomnilnik[7]);
			}
			
			int b = (vrednost & (0x0F0000)) >> 16;
			
			if(b==16){ // Negativna vrednost
				vrednost -= 1048576;
				if(vrednost == -3){
					END = 1;
				}
			}
		}
		
		int UN = getOdmik(nas, x, vrednost);
		//printf("UN = %d\n", UN);
		int konacan  = dodeli(kako, UN, b, p, e, dolzina, ST);
		//printf("konacan = %d\n", konacan);
		return konacan;
}

void upisiVrednost(int vrednost, int kje, int dolzina){
	if(dolzina == WORD){
	
	
		//printf("kje = %d\n", kje);
	
		int a = (vrednost & (0xF00000)) >> 20;
		int b = (vrednost & (0x0F0000)) >> 16;
		int c = (vrednost & (0x00F000)) >> 12;
		int d = (vrednost & (0x000F00)) >> 8;
		int e = (vrednost & (0x0000F0)) >> 4;
		int f = (vrednost & (0x00000F));
		
		//printf("a = %d\n", a);
		//printf("a = %d\n", b);
		//printf("a = %d\n", c);
		//printf("a = %d\n", d);
		//printf("a = %d\n", e);
		//printf("a = %d\n\n", f);
		
		//kje = kje - 1;  // ker se pomnilnik zacne sa 0
	
	
		if(a < 10) { pomnilnik[kje++] = a+48; } else { pomnilnik[kje++] = a+55; }
		if(b < 10) { pomnilnik[kje++] = b+48; } else { pomnilnik[kje++] = b+55; }
		if(c < 10) { pomnilnik[kje++] = c+48; } else { pomnilnik[kje++] = c+55; }
		if(d < 10) { pomnilnik[kje++] = d+48; } else { pomnilnik[kje++] = d+55; }
		if(e < 10) { pomnilnik[kje++] = e+48; } else { pomnilnik[kje++] = e+55; }
		if(f < 10) { pomnilnik[kje++] = f+48; } else { pomnilnik[kje++] = f+55; }
	}
	else{
		int e = (vrednost & (0x0000F0)) >> 4;
		int f = (vrednost & (0x00000F));
		
		if(e < 10) { pomnilnik[kje++] = e+48; } else { pomnilnik[kje++] = e+55; }
		if(f < 10) { pomnilnik[kje++] = f+48; } else { pomnilnik[kje++] = f+55; }
	}

}

void izvrsi(char* trePomnilnik){

	PC += 2;
	
	int32_t znak1 = hexToDec(trePomnilnik[0]);	// Dobijemo znak
	int32_t znak2 = hexToDec(trePomnilnik[1]);
	
	if(ukaz == 1){
		printf("%c%c", trePomnilnik[0], trePomnilnik[1]);
	}

	int a = getBit(znak1, 3);
	int b = getBit(znak1, 2);
	int v = getBit(znak1, 1);
	int g = getBit(znak1, 0);
	int d = getBit(znak2, 3);
	int dj = getBit(znak2, 2);
	
	int e = getBit(znak2, 1); 
	int z = getBit(znak2, 0);
	
	int vrednost3ili4 = a*128 + b*64 + v*32 + g*16 + d*8 + dj*4 + e*0 + z*0;
	int vrednost1ili2 = a*128 + b*64 + v*32 + g*16 + d*8 + dj*4 + e*2 + z*1;
	
	
	//printf("vrednost3ili4 = %d\n", vrednost3ili4 );
	//printf("vrednost1ili2 = %d\n\n", vrednost1ili2 );
	
	//UKAZI iz formata 1
	if(vrednost1ili2 == 196){ //FIX  
		A = (int) F;
		printf("\n");
	}
	else if(vrednost1ili2 == 192){ // FLOAT
		F = A * 1.0;
		printf("\n");
	}
	else if(vrednost1ili2 == 244){ // HIO
	
	}
	else if(vrednost1ili2 == 200){ // NORM
	
	}
	else if(vrednost1ili2 == 240){ // SIO
	
	}
	else if(vrednost1ili2 == 248){ // TIO
	
	}
	//UKAZI iz formata 2
	else if(vrednost1ili2 == 144){ // ADDR r1, r2
		PC += 2;
		int *prviRegister;
		int *drugiRegister;
		
		
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		drugiRegister = getRegister(getBit(znak3, 3), getBit(znak3, 2), getBit(znak3, 1), getBit(znak3, 0));
		
		*drugiRegister += *prviRegister;
	} 
	else if(vrednost1ili2 == 180){ // CLEAR r1
		PC += 2;
		int *prviRegister;
		
		int32_t znak2 = hexToDec(trePomnilnik[2]);
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], '_');
		}
		*prviRegister = 0;
	}
	else if(vrednost1ili2 == 156){ // DIVR r1, r2
		PC += 2;
		int *prviRegister;
		int *drugiRegister;
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		drugiRegister = getRegister(getBit(znak3, 3), getBit(znak3, 2), getBit(znak3, 1), getBit(znak3, 0));
		*drugiRegister /= *prviRegister;
	} 
	else if(vrednost1ili2 == 160){ // COMPR r1, r2
		PC += 2;
		int *prviRegister;
		int *drugiRegister;
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		drugiRegister = getRegister(getBit(znak3, 3), getBit(znak3, 2), getBit(znak3, 1), getBit(znak3, 0));
		if(*prviRegister == *drugiRegister){
			SW = 0;
		}
		else if(*prviRegister < *drugiRegister){
			SW = -1;
		}
		else if(*prviRegister > *drugiRegister){
			SW = 1;
		}
	}
	else if(vrednost1ili2 == 152){ // MULR r1, r2 [-](nije testirano)
		PC += 2;
		int *prviRegister;
		int *drugiRegister;
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		drugiRegister = getRegister(getBit(znak3, 3), getBit(znak3, 2), getBit(znak3, 1), getBit(znak3, 0));
		*drugiRegister *= *prviRegister;
	}
	else if(vrednost1ili2 == 172){ // RMO r1, r2 [-]
		PC += 2;
		int *prviRegister;
		int *drugiRegister;
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		drugiRegister = getRegister(getBit(znak3, 3), getBit(znak3, 2), getBit(znak3, 1), getBit(znak3, 0));
		*drugiRegister = *prviRegister;
	}
	else if(vrednost1ili2 == 164){ // SHIFTL r1, n
		PC += 2;
		int *prviRegister;
		
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		
		int n = (getBit(znak3, 3))*8 + (getBit(znak3, 2))*4 + (getBit(znak3, 1))*2 + (getBit(znak3, 0)) * 1;
		*prviRegister = *prviRegister << n;
	}
	else if(vrednost1ili2 == 168){ // SHIFTR r1, n
		PC += 2;
		int *prviRegister;
		
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], trePomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		int n = (getBit(znak3, 3))*8 + (getBit(znak3, 2))*4 + (getBit(znak3, 1))*2 + (getBit(znak3, 0)) * 1;
		*prviRegister = *prviRegister >> n;
	}
	else if(vrednost1ili2 == 148){ // SUBR r1, r2
		PC += 2;
		int *prviRegister;
		int *drugiRegister;
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		int32_t znak3 = hexToDec(trePomnilnik[3]);
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], pomnilnik[3]);
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		drugiRegister = getRegister(getBit(znak3, 3), getBit(znak3, 2), getBit(znak3, 1), getBit(znak3, 0));
		*drugiRegister -= *prviRegister;
	}
	else if(vrednost1ili2 == 176){ // SVC n
	
	}
	else if(vrednost1ili2 == 184){ // TIXR r1 PITAJ
		PC += 2;
		X += 1; 
		int *prviRegister;
		int32_t znak2 = hexToDec(trePomnilnik[2]);	// Dobijemo znak
		if(ukaz == 1){
			printf("%c%c\n", trePomnilnik[2], '_');
		}
		prviRegister = getRegister(getBit(znak2, 3), getBit(znak2, 2), getBit(znak2, 1), getBit(znak2, 0));
		X = *prviRegister;
	}
	//UKAZI iz formata 3 ili 4
	else if(vrednost3ili4 == 24){ // ADD m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		if(takojsnje == 1){
			konacan /= 2;
		}
		A += konacan;
	}
	else if(vrednost3ili4 == 88){ // ADDF m
	
	}
	else if(vrednost3ili4 == 64){ // AND m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		A &= konacan;
	}
	else if(vrednost3ili4 == 40){ // COMP m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		if(takojsnje == 1){
			konacan /= 2;
		}
		if(A == konacan){
			SW = 0;
		}
		else if(A < konacan){
			SW = -1;
		}
		else if(A > konacan){
			SW = 1;
		}
	}
	else if(vrednost3ili4 == 136){ // COMPF m
	
	}
	else if(vrednost3ili4 == 36){ // DIV m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		if(takojsnje == 1){
			konacan /= 2;
		}
		A /= konacan;
	}
	else if(vrednost3ili4 == 100){ // DIVF m
	
	}
	else if(vrednost3ili4 == 60){ // J m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		PC = konacan;
	}
	else if(vrednost3ili4 == 48){ // JEQ m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		if(SW == 0 ){
			PC = konacan;
		}
	}
	else if(vrednost3ili4 == 52){ // JGT m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		if(SW == 1 ){
			PC = konacan;
		}
	
	}
	else if(vrednost3ili4 == 56){ // JLT m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		if(SW == -1 ){
			PC = konacan;
		}
	}
	else if(vrednost3ili4 == 72){ // JSUB m								
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		
		//printf("JSUB = %d\n", konacan);
		
		L = PC;
		PC = konacan;
	
	}
	else if(vrednost3ili4 == 0){ // LDA m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		//printf("unet = %d\n", konacan);
		if(takojsnje == 1){
			konacan /= 2;
		}
		A = konacan;		
	}
	else if(vrednost3ili4 == 104){ // LDB m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		/*if(takojsnje == 1){
			konacan /= 2;
		}*/
		B = konacan;
	}
	else if(vrednost3ili4 == 80){ // LDCH m   <----------------------------------------------------
		int konacan = ukazi3in4(trePomnilnik, e, z, BYTE, 0);
		A = konacan;
	}
	else if(vrednost3ili4 == 112){ // LDF m
	
	}
	else if(vrednost3ili4 == 8){ // LDL m
		
		//printf("LDL\n");
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		//printf("1 = %d\n", konacan);
		/*if(takojsnje == 1){
			konacan /= 2;
		}*/
		//printf("2 = %d\n", konacan);
		
		// NAOPAKA
		
		
		L = konacan;
	}
	else if(vrednost3ili4 == 108){ // LDS m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		//printf("konacan = %d\n", konacan);
		S = konacan;
	}
	else if(vrednost3ili4 == 116){ // LDT m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		T = konacan;
	}
	else if(vrednost3ili4 == 4){ // LDX m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		X = konacan;
	}
	else if(vrednost3ili4 == 208){ // LPS m
	
	}
	else if(vrednost3ili4 == 32){ // MUL m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		if(takojsnje == 1){
			konacan /= 2;
		}
		A *= konacan;
	}
	else if(vrednost3ili4 == 96){ // MULF m
	
	}
	else if(vrednost3ili4 == 68){ // OR m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		A |= konacan;
	}
	else if(vrednost3ili4 == 216){ // RD m
	
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		FILE *fptr;
		konacan /= 2;
		int vrstica = konacan;
		char* ime = calloc(15, sizeof(char));
		int j = 0;
		int index = 0;
		char* hex = calloc(50, sizeof(char));
		while (konacan != 0) {
			int r = konacan % 16;
			if (r < 10) {
			    hex[index] = r + 48;
			} 
			else{
			    hex[index] = r + 55;
			}
			
			index++;
			konacan = konacan / 16;
		}
		
		for (int i = index - 1; i >= 0; i--) {
		    ime[j++] = hex[i];
		}
		ime[j++] = '.';
		ime[j++] = 'd';
		ime[j++] = 'e';
		ime[j++] = 'v';
		fptr = fopen(ime, "r");
		
		if(fptr == NULL){
			printf("FILE NE OBSTAJA\n");
		}
		
		char* niz = readFromFile(fptr, vrstica);
		
		//printf("%s\n", niz);
		
		int s = strlen(niz)-1;
		A = 0;
		for(int i=0; i<strlen(niz); i++){
			A += hexToDec(niz[i]) * stepen(10, s--);
		}
	
		//printf("vnos = %d\n", A);
	
	
	}
	else if(vrednost3ili4 == 76){ // RSUB m 	<-------------------------------------------------------------
		
		PC = L;	
		if(ukaz == 1){
			printf("%c%c%c%c\n", trePomnilnik[2], trePomnilnik[3], trePomnilnik[4], trePomnilnik[5]);
		}
		
	}
	else if(vrednost3ili4 == 236){ // SSK m
	
	}
	else if(vrednost3ili4 == 12){ // STA m
	
		//printf("ALO\n");
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		/*if(takojsnje == 1){
			konacan /= 2;
		}*/
		//printf("kje = %d\n", konacan);
		upisiVrednost(A, konacan, WORD);
	
	}
	else if(vrednost3ili4 == 120){ // STB m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		upisiVrednost(B, konacan, WORD);
	}
	else if(vrednost3ili4 == 84){ // STCH m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		upisiVrednost(A, konacan, BYTE);
	}
	else if(vrednost3ili4 == 128){ // STF m
	
	}
	else if(vrednost3ili4 == 212){ // STI m
	
	}
	else if(vrednost3ili4 == 20){ // STL m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		/*if(takojsnje == 1){
			konacan /= 2;
		}*/
		upisiVrednost(L, konacan, WORD);
	}
	else if(vrednost3ili4 == 124){ // STS m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		upisiVrednost(S, konacan, WORD);
	}
	else if(vrednost3ili4 == 232){ // STSW m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		upisiVrednost(SW, konacan, WORD);
	}
	else if(vrednost3ili4 == 132){ // STT m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		upisiVrednost(T, konacan, WORD);
	}
	else if(vrednost3ili4 == 16){ // STX m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 1);
		upisiVrednost(X, konacan, WORD);
	}
	else if(vrednost3ili4 == 28){ // SUB m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		if(takojsnje == 1){
			konacan /= 2;
		}
		A -= konacan;
	}
	else if(vrednost3ili4 == 92){ // SUBF m
	
	}
	else if(vrednost3ili4 == 224){ // TD m
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		SW = 1;
	}
	else if(vrednost3ili4 == 44){ // TIX m
	
	}
	else if(vrednost3ili4 == 220){ // WD m
	
		//printf("ispis = %d\n", A);
	
		int konacan = ukazi3in4(trePomnilnik, e, z, WORD, 0);
		FILE *fptr;
		konacan /= 2; 
		
		char* niz1 = calloc(25, sizeof(char));
		char* niz2 = calloc(25, sizeof(char));
		int kje = 0;
		
		int v = A;
		while (v != 0) {
			int r = v % 10;
			niz1[kje] = r + 48;
			kje++;
			v /= 10;
		}
		int j=0;
		for (int i = kje - 1; i >= 0; i--) {
		    niz2[j++] = niz1[i];
		}
		niz2[j++] = '\n';
		
		if(konacan < 256 && konacan!= 0 && konacan!= 1 && konacan!=2){
			char* ime = calloc(15, sizeof(char));
			int j = 0;
			int index = 0;
			char* hex = calloc(50, sizeof(char));
			while (konacan != 0) {
				int r = konacan % 16;
				if (r < 10) {
				    hex[index] = r + 48;
				} 
				else{
				    hex[index] = r + 55;
				}
				
				index++;
				konacan = konacan / 16;
			}
			
			for (int i = index - 1; i >= 0; i--) {
			    ime[j++] = hex[i];
			}
			ime[j++] = '.';
			ime[j++] = 'd';
			ime[j++] = 'e';
			ime[j++] = 'v';
			if(WD[konacan] == 0){
				fptr = fopen(ime, "w");
				WD[konacan]++;
			}
			else{
				fptr = fopen(ime, "a");
			}
			
			
			
			fprintf(fptr, "%s", niz2);
			fflush(fptr);
			fclose(fptr);
			
			free(hex);
			free(ime);
		}
		else if(konacan == 0){
			fprintf(stdin, "%s", niz2);
			fflush(stdin);
		}
		else if(konacan == 1){
			fprintf(stdout, "%s", niz2);
			fflush(stdout);
		}
		else if(konacan == 2){
			fprintf(stderr, "%s", niz2);
			fflush(stderr);
		}
		free(niz1);
		free(niz2);
	}
}



int main(int argc, char* argv[]){
	
	
	FILE *fptr;
	fptr = fopen("Koda.txt", "r"); 
	
	int index = ucitaj(fptr); // index dobija vrednost velikost binarnog citanja koliko bitova je v fajlu
	

	
	
	//int c = preberi(pomnilnik, 11, 20);
	//printf("%d\n",c);
	
	
	//printf("\n\n");
	
	//print(&pomnilnik[226], index);
	
	
	int p = 0;
	
	int start = 0;
	int korak = 0;
	int loop = 1;
	int pom = 0;
	
	printf("ENTER MODE (1-start, 2-step): ");
	scanf("%d", &start); // 1-start, 2-step
	//printf("start = %d\n", start);	
	
	printf("Pomnilnik? (1-Yes, 2-No): ");
	scanf("%d", &pom);
	if(pom == 1){
		print(pomnilnik, index);
	}
	
	
	if(start == 2){
		korak = 2;
	}
	
	
	while(1){
		//printf("p = %d\n", p);
		//printf("PC = %d\n", PC);
		if(korak == 2 && start == 2){
			printf("SUB MODE(1-start, 2-step): ");
			scanf("%d", &korak);	// 1-start, 2-step
			ukaz = 1;
		}
		if(korak == 1){
			start = 1;
			ukaz = 0;
		}
		izvrsi(&pomnilnik[PC]);
		//printf("A = %d\n\n", A);
		if(END == 1){
			break;
		}
		/*if(PC/2 == 40){
			break;
		}*/
		p++;
		ukaz = 0;
	}
	
		printf("\nRegistri: \n");
		printf("A = %d\n", A);
		printf("X = %d\n", X);
		printf("L = %d\n", L);
		printf("S = %d\n", S);
		printf("T = %d\n", T);
		printf("B = %d\n", B);
		printf("SW = %d\n", SW);
		printf("F = %f\n", F);
		printf("PC = %d\n", PC);
		
		printf("Pomnilnik? (1-Yes, 2-No): ");
		scanf("%d", &pom);
		if(pom == 1){
			print(pomnilnik, index);
		}
		
		int st = 0;
		printf("Stevilo korakov? (1-Yes, 2-No): ");
		scanf("%d", &st);
		if(st == 1){
			printf("Stevilo korakov: %d\n", p);
		}
		
		
		//print(&pomnilnik[6712], index);
		
		//printf("%d\n",p);

		//print(pomnilnik, index);
	
	
	return 0;
}





























