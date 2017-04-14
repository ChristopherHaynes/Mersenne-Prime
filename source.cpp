//Mersenne Prime Program - SDFD211 Assignment - Christopher Haynes
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <time.h>
#define MAXSIZE 10000	//Constant used to set the size of the arrays	
using namespace std;

void ClearArray(int x[], int multiplier);	//Resets the array passed to it, up to a max value of (MAXSIZE * multiplier)
void SquareA(void);							//Square the binary value stored in the a[] array and store the result in the b[] array
void SubtractTwo(void);						//Subtract two from the binary value of the b[] array
void ModB(void);							//Mod the value of the b[] array by the value of p
void CheckPrime(void);						//Check whether the value in the b[] array is a mersenne prime
void GeneratePrimes(void);					//Use my own sieve of Eratosthenes to generate all the prime numbers between 1 - 100,000

DWORD currentTime, endTime;													//Double words used to calculate the time passed whilst the program is running
int a[MAXSIZE], b[MAXSIZE * 2], primes[MAXSIZE * 10], oddPrimes[MAXSIZE];	//Two arrays for Mersenne calculations, two arrays for Sieve of Eratosothenes
int p = 3, primesTotal = 1, arrayCounter = 2;								//p is the current prime number being used, primesTotal is the total number of Mersenne Primes found,
																			//arrayCounter is used to keep track of which position in the oddPrimes[] array was last used.
																			//(primesTotal is set to 1 to account for not checking when Mp = 2, which is just counted as a known Mp)
void main(void) 
{
	GeneratePrimes(); //First generate all the prime numbers between 0 - 100,000

	currentTime = GetTickCount(); //Start the timer
	endTime = GetTickCount() + (1000 * 60 * 10);	//Set the endTime to be 10 minutes after the start time 
													//1000 milliseconds times 60 seconds times 10 minutes (total 10 mins)

	while (currentTime <= endTime) { //Main program loop (do for 10 mins)

		currentTime = GetTickCount(); //Update the currentTime

		ClearArray(a, 1); //Empty both of the arrays
		ClearArray(b, 2);

		a[2] = 1; //Set the value of "s" back to 4 in binary (...100)

		for (int i = 0; i < p - 2; i++) {	//repeat the method to p-2 times (would be p-1 if the array wasn't 0 counting)
											//This loops contains the Lucas-Lehmer test - (Sn = S^2n-1 - 2 (mod Mp) Where S0 = 4)
			SquareA();						//Square the value in the a[] array (our current S value)  and store the result in the b[] array

			ClearArray(a, 1);				//Clear array a[] for the next run

			SubtractTwo();					//Subtract 2 from that value
			
			ModB();							//Mod the value of the b[] array (our S^2 - 2) by Mp
			
			ClearArray(b, 2);				//Clear array b[] for the next run
		}

		CheckPrime();						//Check to see if the tested p value was a Mersenne Prime
		p = oddPrimes[arrayCounter];		//Set the p value to the next prime we have not yet tested
		arrayCounter++;						//Increment the arrayCounter for the next test
	}

	cout << "\nTotal Primes Found = " << primesTotal << "\n"; //After the 10 minutes has passed print out the total number of primes found
	_getch();
}

//-----------------------------------------------------------------------------
// Name: GeneratePrimes()
// Desc: Uses the sieve of Eratosothenes to find and store all the
//		 prime numbers between 0 and 100,000
//-----------------------------------------------------------------------------
void GeneratePrimes(void)																										
{
	primes[0] = 1;
	primes[1] = 1;
	int pCheck = 2;
	do {
		
		if(primes[pCheck]==0){ //Mark any multiple of another number as 1 to show it is not prime
		
			for (int i = pCheck * pCheck; i < 100000; i += pCheck){

			primes[i] = 1;
			}
		}
		pCheck++;

	} while (pCheck <= 316);

	int primeCount = 0;

	for (int i = 0; i <= 100000; i++) //Check the list and record the prime values (0) in the oddPrimes array
	{
		if (primes[i] == 0) { oddPrimes[primeCount] = i; primeCount++; }
	}
}

//-----------------------------------------------------------------------------
// Name: CheckPrime()
// Desc: Evaluates the binary array to determine if the resultant number
//		 is a Mersenne Prime
//-----------------------------------------------------------------------------
void CheckPrime(void)
{
	int flag = 0;	//Flag is used a boolean switch to mark if a number is a Mersenne Prime or not 

	for (int i = 0; i < p; i++) { //The loop checks through every position in the array up to p (after the mod function the binary value will be no longer than p digits)
	
		if (a[i] == 0) { flag = 1; break; } //If any position is a zero then Mp is not a Mersenne Prime, set the flag to true and break out of the loop
	}

	if (flag == 0) { cout << "When p=" << p << " Mp is PRIME\n";  primesTotal++; }	//If all the positions are ones the flag remains false and the number is a Prime, print it.
	if (flag == 1) { cout << "When p=" << p << " Mp is NOT PRIME\n"; }				//If a zero is found print that Mp is not a Prime
}

//-----------------------------------------------------------------------------
// Name: ModB()
// Desc: Performs a binary mod function on the value stored in the b[] array
//		 and stores the result in the a[] array
//-----------------------------------------------------------------------------
void ModB(void)
{									//The binary Mod function uses the equivalence (s is equivalent to (s mod 2^p) + |s/2^p| (mod 2^p -1)
									//By taking p of the bits from the least signifcant bit and and adding them to the bits from
									//p to 2p and repeating until I have covered the entire range I produce the result of b[] mod Mp

	int primeIncrement = 0;			//The primeIncrement keeps count of the index where the next block of binary bits starts in the b[] array

	while (primeIncrement <= MAXSIZE){	//Run this loop until the entire array has been covered
	
		for (int i = 0; i < p; i++){	//Loop covering the length of p (with p being not inclusive)
		
			a[i] += b[i + primeIncrement];	//Add the next set of bits from the b[] array into the least significant bits of the a[] array

			if (a[i] > 1) {	//Handle any numbers out of the binary range (> 1), no number will ever be bigger than 2

				a[i] -= 2;	//Reduce the value by two, which always result in either 0 or 1
				a[i + 1]++; //Increase the next position in the array by 1 so that the binary number is accuratly represented
			}
		}

		primeIncrement += p; //Increment the primeIncrement by p so that the next loop starts p places ahead of the last one
	} 
}

//-----------------------------------------------------------------------------
// Name: SubtractTwo()
// Desc: Subratact 2 from the value stored in the b[] array
//-----------------------------------------------------------------------------
void SubtractTwo(void)
{
	int numHold = 0;  //Temporary storage for the first discovered 1 in our binary number used to form a loop

	if (b[1] == 1) { b[1] = 0; } //If the binary 2's position is true, simply set to false

	else {
		for (int i = 2; i < MAXSIZE * 2; i++) { //Loop from the first unchecked binary position up to the end of the array

			if (b[i] == 1) { //Find the next available 1 and record it

				b[i] = 0;			//Set that 1 value back to 0
				numHold = i;		//Set the numHold to the index of the position we just changed from 0 to 1
				break;				//Now the 1 has been found we can break out the loop
			}
		}

		for (int i = numHold; i > 1; i--) { //Loop back to b[1] (binary value of 2) from the last position changed

			b[i - 1] = 1; //Set every previous value from 0 to 1
		}
	}
}

//-----------------------------------------------------------------------------
// Name: SquareA()
// Desc: Square the binary value stored in the a[] array and store in the b[] array
//-----------------------------------------------------------------------------
void SquareA(void)
{						//The binary squaring function works on the basis that if I double the value of an index position, that
						//new index position reprents the square value of the first index position. 
						//E.G 000100 (4)(index of first one = 2) squared is 010000 (16)(index of new position = 4) 
						//By adding the index positions together the values are being effectivly multiplied
						//By repeating this for every 1 in the array; the number is squared by a means of long mulitplication

	for(int i = 0; i < MAXSIZE; i++){	//Outer loop for every position in the a[] array
    
		if(a[i] == 1){ //We only need to square the value of a bit if it equals one

			for(int j = 0; j < MAXSIZE; j++){ //Inner loop for squaring the discovered one value

				if(a[j] == 1) { //Find the next one

					b[i + j]++; //Go to the the discovered one's array position plus the original discovered one's array position in the b[] array
				}				//Increment that value by 1

				if (b[i + j] > 1) { //Check to make sure the number is still in the binary range

					b[i + j] -= 2; //Reduce the value by two (results in either 0 or 1)
					b[i + j + 1]++; //Increment the next position by one so the representation of the binary number is still correct
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: ClearArray()
// Desc: Set all positions in the given array, up to a maximum of 
//		 MAXSIZE * multiplier, back to 0
//-----------------------------------------------------------------------------
void ClearArray(int x[], int multiplier)
{
	for(int i = 0; i < MAXSIZE * multiplier; i ++){	//Loop for every position of the array, multiplier is used so the same function can be used with multiple arrays
	
		x[i] = 0; //Set each position back to zero
	}
}