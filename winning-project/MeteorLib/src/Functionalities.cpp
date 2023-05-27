#pragma once
#include "Functionalities.h"
#include "Precompute.h"
#include <thread>
// #include <stdlib.h>
// #define random(x) (rand()%X)


using namespace std;
extern Precompute PrecomputeObject;
extern string SECURITY_TYPE;

/******************************** Functionalities 2PC ********************************/
// Share Truncation, truncate shares of a by power (in place) (power is logarithmic)

void funcReconstructBit(const RSSVectorSmallType &a, vector<smallType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: RSSSmallType (bits), smallType (bit)");


		vector<smallType> a_next(size), a_prev(size);
		for (int i = 0; i < size; ++i)
		{
			a_prev[i] = 0;
			a_next[i] = a[i].first;
			b[i] = a[i].first;
			b[i] = b[i] ^ a[i].second;
		}

		thread *threads = new thread[2];
		threads[0] = thread(sendVector<smallType>, ref(a_next), nextParty(partyNum), size);
		threads[1] = thread(receiveVector<smallType>, ref(a_prev), prevParty(partyNum), size);
		for (int i = 0; i < 2; i++)
			threads[i].join();
		delete[] threads;

		for (int i = 0; i < size; ++i)
			b[i] = b[i] ^ a_prev[i];

		if (print)
		{
			std::cout << str << ": \t\t";
			for (int i = 0; i < size; ++i)
				cout << (int)(b[i]) << " "; 
			std::cout << std::endl;
		}
	
}

void funcReconstruct(const RSSVectorSmallType &a, vector<smallType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: RSSSmallType, smallType");


		vector<smallType> a_next(size), a_prev(size);
		for (int i = 0; i < size; ++i)
		{
			a_prev[i] = 0;
			a_next[i] = a[i].first;
			b[i] = a[i].first;
			b[i] = additionModPrime[b[i]][a[i].second];
		}

		thread *threads = new thread[2];

		threads[0] = thread(sendVector<smallType>, ref(a_next), nextParty(partyNum), size);
		threads[1] = thread(receiveVector<smallType>, ref(a_prev), prevParty(partyNum), size);

		for (int i = 0; i < 2; i++)
			threads[i].join();

		delete[] threads;

		for (int i = 0; i < size; ++i)
			b[i] = additionModPrime[b[i]][a_prev[i]];

		if (print)
		{
			std::cout << str << ": \t\t";
			for (int i = 0; i < size; ++i)
				cout << (int)(b[i]) << " "; 
			std::cout << std::endl;
		}
	
}

void funcReconstruct(const RSSVectorMyType &a, vector<myType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: RSSMyType, myType");
	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<myType> a_next(size), a_prev(size, 0);
	for (int i = 0; i < size; ++i)
	{
		a_next[i] = a[i].first;
		b[i] = a[i].first + a[i].second;
	}

		thread *threads = new thread[2];

		threads[0] = thread(sendVector<myType>, ref(a_next), nextParty(partyNum), size);
		threads[1] = thread(receiveVector<myType>, ref(a_prev), prevParty(partyNum), size);

		for (int i = 0; i < 2; i++)
			threads[i].join();

		delete[] threads;

		for (int i = 0; i < size; ++i)
			b[i] = b[i] + a_prev[i];

		if (print)
		{
			std::cout << str << ": \t\t";
			for (int i = 0; i < size; ++i)
				print_linear(b[i], "SIGNED");
			std::cout << std::endl;
		}
	
}


/******************************** Functionalities MPC ********************************/
// Matrix Multiplication of a*b = c with transpose flags for a,b.
// Output is a share between PARTY_A and PARTY_B.
// a^transpose_a is rows*common_dim and b^transpose_b is common_dim*columns
// define Meteor MatMul
void Meteor_funcMatMul(const MEVectorType &a, const MEVectorType &b, MEVectorType &c, 
					size_t rows, size_t common_dim, size_t columns,
				 	size_t transpose_a, size_t transpose_b, size_t truncation)
{
	log_print("Meteor_funcMatMul");
	cout << "WE ARE USING METEOR MATMUL!" << endl;
	assert(a.size() == rows*common_dim && "Matrix a incorrect for Mat-Mul");
	assert(b.size() == common_dim*columns && "Matrix b incorrect for Mat-Mul");
	assert(c.size() == rows*columns && "Matrix c incorrect for Mat-Mul");

	size_t final_size = rows*columns;
	RSSVectorMyType temp3(final_size);
	vector<myType> Delta(final_size, 0);

	matrixMultMeteor(a, b, temp3, rows, common_dim, columns, transpose_a, transpose_b);
	
	funcReconstruct(temp3, Delta, final_size, "Delta Reconst", false);

	dividePlain(Delta, (1<<truncation));

	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < final_size; ++i)
		{
			c[i] = make_pair(Delta[i], make_pair(0,0));
			//cout << c[i].first << endl;;
		}
	}
	if (partyNum == PARTY_B)
	{
		for (int i = 0; i < final_size; ++i)
		{
			c[i] = make_pair(Delta[i], make_pair(0,0));
		}
	}

	if (partyNum == PARTY_C)
	{
		for (int i = 0; i < final_size; ++i)
		{
			c[i] = make_pair(Delta[i], make_pair(0,0));
		}
	}	
}

// 2-input dot product, used in private compare and relu.
void Meteor_funcDotProduct(const MEVectorType &a, const MEVectorType &b, MEVectorType &c, size_t size, bool truncation, size_t precision){

	log_print("Meteor Function for Dot Product");
	assert(a.size() == size && "a size is incorrect");
	assert(b.size() == size && "b size is incorrect");
	assert(c.size() == size && "c size is incorrect");

	RSSVectorMyType temp3(size);
	vector<myType> Delta(size, 0), temp4(size, 0);
	
	for (int i = 0; i < size; i++){
		if(partyNum == PARTY_A){
			temp3[i].first = a[i].first * b[i].first + a[i].first * b[i].second.first + a[i].second.first * b[i].first + 0 - 0;
			temp3[i].second = a[i].first * b[i].second.second + a[i].second.second * b[i].first + 0 - 0;
		}
		else if(partyNum == PARTY_B){
			temp3[i].first = a[i].first * b[i].second.first + a[i].second.first * b[i].first + 0 - 0;
			temp3[i].second = a[i].first * b[i].second.second + a[i].second.second * b[i].first + 0 - 0;
		}
		else{
			temp3[i].first = a[i].first * b[i].second.first + a[i].second.first * b[i].first + 0 - 0;
			temp3[i].second = a[i].first * b[i].first + a[i].first * b[i].second.second + a[i].second.second * b[i].first + 0 - 0;			
		}
		temp4[i] = temp3[i].second;

	}
	//funcReconstruct(temp3, Delta, size, "Delta Reconst", false);

	
	thread *threads = new thread[2];

	threads[0] = thread(sendVector<myType>, ref(temp4), prevParty(partyNum), size);
	threads[1] = thread(receiveVector<myType>, ref(Delta), nextParty(partyNum), size);
		
	for (int i = 0; i < 2; i++)
		threads[i].join();
	delete[] threads; 

	for(int i = 0; i < size; i++){
		Delta[i] = Delta[i] + temp3[i].first + temp3[i].second;
	}
	
	if(truncation)
		dividePlain(Delta, (1<<truncation));

	
	for (int i = 0; i < size; ++i)
	{
		c[i] = make_pair(Delta[i], make_pair(0,0));
		//c[i] = make_pair(Delta[i]*0.5, make_pair(0,0));
		//c[i].second.first = 0;
		//c[i].second.second = 0;
			//cout << c[i].first << endl;;
	}


}

void funcReconstruct3out3parallel(const vector<smallType> &a, vector<smallType> &b, size_t size, string str, bool print)
{
 	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<smallType> a_next(size), a_prev(size);
 	for (int i = 0; i < size; ++i)
 	{
 		a_prev[i] = 0;
 		a_next[i] = 0;
 	}

 	thread *threads = new thread[4];

 	threads[0] = thread(sendVector<smallType>, ref(a), nextParty(partyNum), size);
 	threads[1] = thread(sendVector<smallType>, ref(a), prevParty(partyNum), size);
 	threads[2] = thread(receiveVector<smallType>, ref(a_next), nextParty(partyNum), size);
 	threads[3] = thread(receiveVector<smallType>, ref(a_prev), prevParty(partyNum), size);

 	for (int i = 0; i < 4; i++)
 		threads[i].join();

 	delete[] threads;

 	for (int i = 0; i < size; ++i)
 		b[i] = additionModPrime[additionModPrime[a[i]][a_prev[i]]][a_next[i]];
 }

void funcReconstruct3out3(const vector<smallType> &a, vector<smallType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: smallType, smallType");
	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<smallType> temp_A(size,0), temp_B(size, 0);

	if (partyNum == PARTY_A or partyNum == PARTY_B)
		sendVector<smallType>(a, PARTY_C, size);

	if (partyNum == PARTY_C)
	{
		receiveVector<smallType>(temp_A, PARTY_A, size);
		receiveVector<smallType>(temp_B, PARTY_B, size);
		addVectors<smallType>(temp_A, a, temp_A, size);
		addVectors<smallType>(temp_B, temp_A, b, size);
		sendVector<smallType>(b, PARTY_A, size);
		sendVector<smallType>(b, PARTY_B, size);
	}

	if (partyNum == PARTY_A or partyNum == PARTY_B)
		receiveVector<smallType>(b, PARTY_C, size);

	if (print)
	{
		std::cout << str << ": \t\t";
		for (int i = 0; i < size; ++i)
			print_linear(b[i], "SIGNED");
		std::cout << std::endl;
	}
}


void funcReconstruct3out3parallel(const vector<myType> &a, vector<myType> &b, size_t size, string str, bool print)
{
 	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<myType> a_next(size), a_prev(size);
 	for (int i = 0; i < size; ++i)
 	{
 		a_prev[i] = 0;
 		a_next[i] = 0;
 	}

 	thread *threads = new thread[4];

 	threads[0] = thread(sendVector<myType>, ref(a), nextParty(partyNum), size);
 	threads[1] = thread(sendVector<myType>, ref(a), prevParty(partyNum), size);
 	threads[2] = thread(receiveVector<myType>, ref(a_next), nextParty(partyNum), size);
 	threads[3] = thread(receiveVector<myType>, ref(a_prev), prevParty(partyNum), size);

 	for (int i = 0; i < 4; i++)
 		threads[i].join();

 	delete[] threads;

 	for (int i = 0; i < size; ++i)
 		b[i] = a[i] + a_prev[i] + a_next[i];
 }

void funcReconstruct3out3(const vector<myType> &a, vector<myType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: myType, myType");
	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<myType> temp_A(size,0), temp_B(size, 0);

	if (partyNum == PARTY_A or partyNum == PARTY_B)
		sendVector<myType>(a, PARTY_C, size);

	if (partyNum == PARTY_C)
	{
		receiveVector<myType>(temp_A, PARTY_A, size);
		receiveVector<myType>(temp_B, PARTY_B, size);
		addVectors<myType>(temp_A, a, temp_A, size);
		addVectors<myType>(temp_B, temp_A, b, size);
		sendVector<myType>(b, PARTY_A, size);
		sendVector<myType>(b, PARTY_B, size);
	}

	if (partyNum == PARTY_A or partyNum == PARTY_B)
		receiveVector<myType>(b, PARTY_C, size);

	if (print)
	{
		std::cout << str << ": \t\t";
		for (int i = 0; i < size; ++i)
			print_linear(b[i], "SIGNED");
		std::cout << std::endl;
	}
}


void Meteor_funcDotProduct(const MEVectorSmallType &a, const MEVectorSmallType &b, MEVectorSmallType &c, size_t size)
{
	log_print("2-input meteor dot protudct in prime field");
	assert(a.size() == size && "Vector a incorrect for vec-prod");
	assert(b.size() == size && "vector b incorrect for vec-prod");
	assert(c.size() == size && "vector c incorrect for vec-prod");

	RSSVectorSmallType temp(size);
	vector<smallType> temp4(size, 0);

	for(int i = 0; i < size; i++){
		temp[i].first = multiplicationModPrime[a[i].first][b[i].second.first];
		temp[i].first = additionModPrime[temp[i].first][multiplicationModPrime[a[i].second.first][b[i].first]];

		temp[i].second = multiplicationModPrime[a[i].first][b[i].second.second];
		temp[i].second = additionModPrime[temp[i].second][multiplicationModPrime[a[i].second.second][b[i].first]];

		if(partyNum==PARTY_A)
		{
			temp[i].first =  subtractModPrime[additionModPrime[additionModPrime[multiplicationModPrime[a[i].first][b[i].first]][temp[i].first]][0]][0]; //(multiplicationModPrime[a[i].first][b[i].first] - temp[i].first + 0 + 0) % PRIME_NUMBER;
		}
		else
		{
			temp[i].first =subtractModPrime[additionModPrime[temp[i].first][0]][0]; //(-temp[i].first + 0 + 0) % PRIME_NUMBER;
		}

		if(partyNum==PARTY_C)
		{
			temp[i].second = subtractModPrime[additionModPrime[additionModPrime[multiplicationModPrime[a[i].first][b[i].first]][temp[i].first]][0]][0];
			//(multiplicationModPrime[a[i].first][b[i].first] - temp[i].first + 0 + 0) % PRIME_NUMBER;
		}
		else
		{
			temp[i].second = subtractModPrime[additionModPrime[temp[i].second][0]][0];// (-temp[i].second + 0 + 0) % PRIME_NUMBER;
		}
	}

	vector<smallType> Delta(size, 0);
	funcReconstruct(temp, Delta, size, "2-input small dot-prod Reconst", false);

	for(int i = 0; i < size; i++){
		Delta[i] = additionModPrime[additionModPrime[Delta[i]][temp[i].first]][temp[i].second];
	}

	
	for(int i =0; i < size; i++)
	{
		c[i] = make_pair(Delta[i], make_pair(0,0));
		
	}
}



// Term by term multiplication boolean shares
void Meteor_funcDotProductBits(const MEVectorSmallType &a, const MEVectorSmallType &b, MEVectorSmallType &c, size_t size)
{
	log_print("Meteor funcDotProductBits");
	assert(a.size() == size && "Vector a incorrect for DPB");
	assert(b.size() == size && "Vector b incorrect for DPB");
	assert(c.size() == size && "Vector c incorrect for DPB");
	
	RSSVectorSmallType temp(size);

	for (int i = 0; i < size; i++)
	{
		temp[i].first = (a[i].first and b[i].second.first) ^ (a[i].second.first and b[i].first) ^ 0 ^ 0;
		temp[i].second = (a[i].first and b[i].second.second) ^ (a[i].second.second and b[i].first) ^ 0 ^ 0;
	
		if(partyNum == PARTY_A)
		{
			temp[i].first = temp[i].first ^ (a[i].first and b[i].first);
		}
		if(partyNum == PARTY_C)
		{
			temp[i].second = temp[i].second ^ (a[i].first and b[i].first);
		}
	}
	vector<smallType> Delta(size, 0);
	funcReconstruct(temp, Delta, size, "2-input small bit-prod Reconst", false);
	
	for(int i =0; i < size; i++)
	{
		c[i] = make_pair(Delta[i], make_pair(0,0));
	}
}

void Thunder_funcNMultGateOnline(const MEVectorType &c_1, MEVectorType &c_2, size_t size)
{
	log_print("only support 64 bit!");
	assert((size % 4) == 0 && "size is not in {64, 16, 4}, error!");
	size_t rss_size = size / 2, res_size = size / 4;
	RSSVectorMyType rss_tmp(rss_size, make_pair(0,0));
	vector<myType> ass_tmp(res_size, 0), Delta(res_size, 0);

	for(size_t i = 0; i < rss_size; i++){
	   // masked-replicated secret sharing -> mult-> replicated secret sharing;
		rss_tmp[i].first = c_1[2*i].first * c_1[2*i+1].second.first + c_1[2*i].second.first * c_1[2*i+1].first + 0 - 0;
		rss_tmp[i].second = c_1[2*i].first * c_1[2*i+1].second.second + c_1[2*i].second.second * c_1[2*i+1].first + 0 - 0;

		if(partyNum == PARTY_A){
			rss_tmp[i].first += c_1[2*i].first * c_1[2*i+1].first;
		}
		if(partyNum == PARTY_C){
			rss_tmp[i].second += c_1[2*i].first * c_1[2*i+1].first;
		}
	}

	for(size_t i = 0; i < res_size; i++){
		// replicated secret sharing -> mult -> additive secret sharing;
		ass_tmp[i] = rss_tmp[2*i].first * rss_tmp[2*i+1].first + rss_tmp[2*i].first * rss_tmp[2*i+1].second + rss_tmp[2*i].second * rss_tmp[2*i+1].first;
		//additionModPrime[additionModPrime[multiplicationModPrime[rss_tmp[2*i].first][rss_tmp[2*i+1].first]][multiplicationModPrime[rss_tmp[2*i].first][rss_tmp[2*i+1].second]]][multiplicationModPrime[rss_tmp[2*i].second][rss_tmp[2*i+1].first]];
	}

	funcReconstruct3out3parallel(ass_tmp, Delta, res_size, "reveal 3-3 sharing", false);

	for(size_t i =0; i < res_size; i++)
	{
		c_2[i] = make_pair(Delta[i], make_pair(0,0));
		
	}

}

void Thunder_funcNMultGate(const MEVectorType &c, size_t size, size_t N)
{
	size_t sizeLong = size * N;
	size_t rounds = size_t(log2(N)/2);

	//start_m();
	//start_communication();
	//start_time();
	// Setup for our improved 4-input mult gates over meteor, start
	size_t setup_size = N + N / 4 + N / 16;
	MEVectorType a(setup_size / 2,  make_pair(1, make_pair(0,0))),  b(setup_size / 2,  make_pair(2, make_pair(0,0))), ab(setup_size / 2);
	Meteor_funcDotProduct(a, b, ab, setup_size/2, false, 0);
	// end;

	//end_m("setup for 4-input gates for N fan-in");
	//end_time("setup for 4-mult with N fan-in");
	//pause_communication();
	//end_communication("setup for 4-mult with N fan-in");
  
   //start_communication();
   //start_time();
	vector<MEVectorType> c_tau(rounds);
	for(int i =0; i < rounds; i++){
		c_tau[i] = MEVectorType(sizeLong >> (2*(i+1)), make_pair(0, make_pair(0,0)));
	}
	Thunder_funcNMultGateOnline(c, c_tau[0], sizeLong);
	for(int i = 1; i < rounds; i++){
		Thunder_funcNMultGateOnline(c_tau[i-1], c_tau[i], sizeLong >> (2*i));
	}
	//end_time("online for 4-mult with N fan-in");
	//pause_communication();
	//end_communication("online for 4-mult with N fan-in");

}




void Meteor_funcMultiplyNeighbors(const MEVectorSmallType &c_1, MEVectorSmallType &c_2, size_t size)
{
	log_print("only support 64 bit!");
	assert((size % 4) == 0 && "size is not in {64, 16, 4}, error!");
	size_t rss_size = size / 2, res_size = size / 4;
	RSSVectorSmallType rss_tmp(rss_size, make_pair(0,0));
	vector<smallType> ass_tmp(res_size, 0), Delta(res_size, 0);

	for(size_t i = 0; i < rss_size; i++){
	   // masked-replicated secret sharing -> mult-> replicated secret sharing;
		rss_tmp[i].first = additionModPrime[multiplicationModPrime[c_1[2*i].first][c_1[2*i+1].second.first]][multiplicationModPrime[c_1[2*i].second.first][c_1[2*i+1].first]]+0-0;
		rss_tmp[i].second = additionModPrime[multiplicationModPrime[c_1[2*i].first][c_1[2*i+1].second.second]][multiplicationModPrime[c_1[2*i].second.second][c_1[2*i+1].first]]+0-0;

		if(partyNum == PARTY_A){
			rss_tmp[i].first = additionModPrime[rss_tmp[i].first][multiplicationModPrime[c_1[2*i].first][c_1[2*i+1].first]];
		}
		if(partyNum == PARTY_C){
			rss_tmp[i].second =  additionModPrime[rss_tmp[i].second][multiplicationModPrime[c_1[2*i].first][c_1[2*i+1].first]];
		}
	}

	for(size_t i = 0; i < res_size; i++){
		// replicated secret sharing -> mult -> additive secret sharing;
		ass_tmp[i] = additionModPrime[additionModPrime[multiplicationModPrime[rss_tmp[2*i].first][rss_tmp[2*i+1].first]][multiplicationModPrime[rss_tmp[2*i].first][rss_tmp[2*i+1].second]]][multiplicationModPrime[rss_tmp[2*i].second][rss_tmp[2*i+1].first]];
	}
	funcReconstruct3out3parallel(ass_tmp, Delta, res_size, "reveal 3-3 sharing", false);

	for(size_t i =0; i < res_size; i++)
	{
		c_2[i] = make_pair(Delta[i], make_pair(0,0));
		
	}


	/*
	size_t res_size = size / 4;
	RSSVectorSmallType temp(res_size, make_pair(0,0));
	for (int i = 0; i < res_size; i++)
	{
		temp[i].first = multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i].second.first][c_1[4*i+1].first]][c_1[4*i+2].first]][c_1[4*i+3].first];
		temp[i].first = additionModPrime[temp[i].first][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i+1].second.first][c_1[4*i].first]][c_1[4*i+2].first]][c_1[4*i+3].first]];
		temp[i].first = additionModPrime[temp[i].first][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i+2].second.first][c_1[4*i+1].first]][c_1[4*i].first]][c_1[4*i+3].first]];
		temp[i].first = additionModPrime[temp[i].first][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i+3].second.first][c_1[4*i].first]][c_1[4*i+2].first]][c_1[4*i].first]];
		temp[i].first = subtractModPrime[temp[i].first][0];

		temp[i].second = multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i].second.second][c_1[4*i+1].first]][c_1[4*i+2].first]][c_1[4*i+3].first];
		temp[i].second = additionModPrime[temp[i].second][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i+1].second.second][c_1[4*i].first]][c_1[4*i+2].first]][c_1[4*i+3].first]];
		temp[i].second = additionModPrime[temp[i].second][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i+2].second.second][c_1[4*i+1].first]][c_1[4*i].first]][c_1[4*i+3].first]];
		temp[i].second = additionModPrime[temp[i].second][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i+3].second.second][c_1[4*i+1].first]][c_1[4*i+2].first]][c_1[4*i].first]];
		temp[i].second = subtractModPrime[temp[i].second][0];

		if(partyNum == PARTY_A)
		{
			temp[i].first = additionModPrime[temp[i].first][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i].first][c_1[4*i+1].first]][c_1[4*i+2].first]][c_1[4*i+3].first]];
		}
		if(partyNum == PARTY_C)
		{
			temp[i].second = additionModPrime[temp[i].second][multiplicationModPrime[multiplicationModPrime[multiplicationModPrime[c_1[4*i].first][c_1[4*i+1].first]][c_1[4*i+2].first]][c_1[4*i+3].first]];
		}
	}
	
	vector<smallType> Delta(res_size, 0);
	funcReconstruct(temp, Delta, res_size, "4-input small 4-neighbormultiply Reconst", false);
	
	for(int i =0; i < res_size; i++)
	{
		c_2[i] = make_pair(Delta[i], make_pair(0,0));
		
	}
	*/

}

void Meteor_funcCrunchMultiply(const MEVectorSmallType &c, vector<smallType> &betaPrime, size_t size)
{
	size_t sizeLong = size * BIT_SIZE;
	size_t rounds = size_t(log2(BIT_SIZE)/2);

	//start_m();
	//start_communication();
	//start_time();
	// Setup for our improved 4-input mult gates over meteor, start
	size_t setup_size = BIT_SIZE + BIT_SIZE / 4 + BIT_SIZE / 16;
	MEVectorSmallType a(setup_size / 2,  make_pair(1, make_pair(0,0))),  b(setup_size / 2,  make_pair(2, make_pair(0,0))), ab(setup_size / 2);
	Meteor_funcDotProduct(a, b, ab, setup_size/2);
	// end;

	//end_m("setup for 4-input gates for 64 fan-in");
	//end_time("setup for 4-mult with 64 fan-in");
	//pause_communication();
	//end_communication("setup for 4-mult with 64 fan-in");
  
   //start_communication();
   //start_time();
	vector<MEVectorSmallType> c_tau(rounds);
	for(int i =0; i < rounds; i++){
		c_tau[i] = MEVectorSmallType(sizeLong >> (2*(i+1)), make_pair(0, make_pair(0,0)));
	}
	Meteor_funcMultiplyNeighbors(c, c_tau[0], sizeLong);
	for(int i = 1; i < rounds; i++){
		Meteor_funcMultiplyNeighbors(c_tau[i-1], c_tau[i], sizeLong >> (2*i));
	}
	//end_time("online for 4-mult with 64 fan-in");
	//pause_communication();
	//end_communication("online for 4-mult with 64 fan-in");
	
	MEVectorSmallType m(size, make_pair(1, make_pair(0,0))), prod(size);
	vector<smallType> reconst(size, 0);
	RSSVectorSmallType delta(size);

	Meteor_funcDotProduct(c_tau[rounds-1], m, prod, size);

	for(int i=0; i < size; i++){
		delta[i] = prod[i].second;
	}

	funcReconstruct(delta, reconst, size, "neighbors multiply delta reconst", false);
	
	for(int i = 0; i < size; i++){
		reconst[i] = subtractModPrime[prod[i].first][reconst[i]];
	}
	for(int i =0; i < size; i++){
		if(reconst[i]==0){
			betaPrime[i] = 1;
		}
		//if(partyNum == PARTY_A)
			//cout << int(betaPrime[i]) << endl;

	}

}


void Meteor_funcPrivateCompare(const MEVectorSmallType &share_m, const vector<myType> &r, const MEVectorSmallType &beta, vector<smallType> &betaPrime, size_t size)
{
	log_print("Meteor private compare functionality");
	assert(share_m.size() == size * BIT_SIZE && "input error share_m");
	assert(r.size() == size && "Input error r");
	assert(beta.size() == size && "Input error beta");

	size_t sizeLong = size * BIT_SIZE;
	size_t index3, index2;
	MEVectorSmallType c(sizeLong), diff(sizeLong), twoBetaMinusOne(sizeLong), xMinusR(sizeLong);
	MESmallType a, tempM, tempN;
	smallType bit_r;

	for(int index2 = 0; index2 < size; index2++)
	{
		twoBetaMinusOne[index2*BIT_SIZE].first =  subtractModPrime[additionModPrime[beta[index2].first][beta[index2].first]][1];
		// (beta[index2].first + beta[index2].first - 1) % PRIME_NUMBER;
		twoBetaMinusOne[index2*BIT_SIZE].second.first = additionModPrime[beta[index2].second.first][beta[index2].second.first];
		//(beta[index2].second.first + beta[index2].second.first) % PRIME_NUMBER;
		twoBetaMinusOne[index2*BIT_SIZE].second.second = additionModPrime[beta[index2].second.second][beta[index2].second.second];
		//(beta[index2].second.second + beta[index2].second.second) % PRIME_NUMBER;

		for(size_t k = 0; k < BIT_SIZE; k++)
		{
			index3 = index2 * BIT_SIZE + k;
			twoBetaMinusOne[index3] = twoBetaMinusOne[index2*BIT_SIZE];

			bit_r = (smallType)( (r[index2] >> (BIT_SIZE-1-k)) & 1 );
			diff[index3] = share_m[index3];

			if(bit_r == 1){
				diff[index3].first = subtractModPrime[diff[index3].first][1];
			}
		}
	}
		
		// (-1)^beta * (x[i]-r[i]) in vectorization.
	Meteor_funcDotProduct(diff, twoBetaMinusOne, xMinusR, sizeLong);


	for(int index2 = 0; index2 < size; index2++)
	{
		a = make_pair(0, make_pair(0,0));
		for(size_t k = 0; k < BIT_SIZE; k++)
		{
			index3 = index2 * BIT_SIZE + k;
			c[index3] = a;
			tempM = share_m[index3];

			bit_r = (smallType)((r[index2] >> (BIT_SIZE-1-k)) & 1);

			tempN.first = tempM.first ^ bit_r;
			tempN.second = tempM.second;

			a.first = additionModPrime[a.first][tempN.first];
			a.second = addModPrime(a.second, tempN.second);

			c[index3].first = additionModPrime[additionModPrime[c[index3].first][xMinusR[index3].first]][1];
			c[index3].second = addModPrime(c[index3].second, xMinusR[index3].second);
		}

	}
	Meteor_funcCrunchMultiply(c, betaPrime, size);

}


void Meteor_funcRELUPrime(const MEVectorType &a, MEVectorSmallType &b, size_t size)
{
	log_print("Meteor funcRELUPrime");
	assert(a.size() == size && "input a is size error!");
	//cout << partyNum << endl;

	MEVectorSmallType delta(size*BIT_SIZE, make_pair(1, make_pair(0,0))), beta(size, make_pair(0, make_pair(0,0)));
	vector<myType> Delta(size, 0);
	vector<smallType> betaPrime(size, 0);
	for(int i = 0; i < size; i++)
	{
		Delta[i] = -(a[i].first << 1);
	}
	Meteor_funcPrivateCompare(delta, Delta, beta, betaPrime, size); // ???

	for(int i = 0; i < size; i++)
	{
		b[i].first = getMSB(a[i].first) ^ betaPrime[i] ^ 1;
		b[i].second = delta[i*BIT_SIZE].second; // offline done, just efficiency simulation, not correct!
	}


}


void Meteor_funcRELU(const MEVectorType &a, MEVectorSmallType &temp, MEVectorType &b, size_t size)
{
	log_print("Meteor funcRELU");
	cout << "WE ARE USING METEOR RELU NOW!" << endl;
	assert(a.size() == size && "input a is size error");
	//MEVectorSmallType c(size, make_pair(0, make_pair(0,0)));
	RSSVectorMyType delta_shared(size);
	vector<myType> delta(size);
	//MEVectorType m_c(size, make_pair(0, make_pair(0,0)));
	//vector<smallType> reconst_b(size, 0), a_prev(size, 0), a_next(size, 0);
	
	Meteor_funcRELUPrime(a, temp, size);
	
	for(int i = 0; i < size; i++)
	{
		if(partyNum == PARTY_A){
			delta_shared[i].first = ( 1 - 2 * int(temp[i].second.first) ) * int(temp[i].first) * a[i].first + a[i].first * int(temp[i].second.first) + a[i].second.first * int(temp[i].first) + (1-2*int(temp[i].first)) * 0 - 0;
			delta_shared[i].second = (-2 * int(temp[i].second.second) ) * int(temp[i].first) * a[i].first + a[i].first * int(temp[i].second.second) + a[i].second.second * int(temp[i].first) + (1-2*int(temp[i].first)) * 0 - 0;
		}
		else if(partyNum == PARTY_B){
			delta_shared[i].first = (-2 * int(temp[i].second.first) ) * int(temp[i].first) * a[i].first + a[i].first * int(temp[i].second.first) + a[i].second.first * int(temp[i].first) + (1-2*int(temp[i].first)) * 0 - 0;
			delta_shared[i].second = (-2 * int(temp[i].second.second) ) * int(temp[i].first) * a[i].first + a[i].first * int(temp[i].second.second) + a[i].second.second * int(temp[i].first) + (1-2*int(temp[i].first)) * 0 - 0;
		}
		else if(partyNum == PARTY_C){
			delta_shared[i].first = (-2 * int(temp[i].second.first) ) * int(temp[i].first) * a[i].first + a[i].first * int(temp[i].second.first) + a[i].second.first * int(temp[i].first) + (1-2*int(temp[i].first)) * 0 - 0;
			delta_shared[i].second = (1-2 * int(temp[i].second.second) ) * int(temp[i].first) * a[i].first + a[i].first * int(temp[i].second.second) + a[i].second.second * int(temp[i].first) + (1-2*int(temp[i].first)) * 0 - 0;
		}
	}
	funcReconstruct(delta_shared, delta, size, "delta", false);

	for(int i = 0; i < size; i++){
		b[i] = make_pair(delta[i], make_pair(0,0));
	}
	
	/*
	for(int i = 0; i < size; ++i)
	{
		bXORc[i] = c[i].second ^ temp[i].second;
		//bXORc[i].second = c[i].second.second ^ temp[i].second.second;
		if(partyNum == PARTY_A)
		{
			bXORc[i].first = bXORc[i].first ^ c[i].first ^ temp[i].first;
		}
		if(partyNum == PARTY_C)
		{
			bXORc[i].second = bXORc[i].second ^ c[i].first ^ temp[i].first;
		}
	}
	
	funcReconstructBit(bXORc, reconst_b, size, "bXORc", false);

	
	for(int i = 0; i < size; i++)
	{
		if(reconst_b[i] == 0)
		{
			m_c[i].first = (myType)1 - m_c[i].first;
			m_c[i].second.first = -m_c[i].second.first;
			m_c[i].second.second = - m_c[i].second.second;
		}
	}

	// b = m_c * a
	Meteor_funcDotProduct(a, m_c, b, size, false, 0);
	*/
	

}


//Chunk wise maximum of a vector of size rows*columns and maximum is caclulated of every 
//column number of elements. max is a vector of size rows, maxPrime, of rows*columns*columns; 
void Meteor_funcMaxpool(const MEVectorType &a, MEVectorType &max,
						 size_t rows, size_t columns)
{
	log_print("funcMaxpool");
	assert(columns < 256 && "Pooling size has to be smaller than 8-bits");

	size_t size = rows*columns;
	MEVectorType diff(rows);
	MEVectorSmallType rp(rows); // dmpIndexShares(columns*size), temp(size);

	for (size_t i = 0; i < rows; ++i)
	{	
		max[i] = a[i*columns];
		//max[i].second = a[i*columns].second;
	}
	for (size_t i = 1; i < columns; ++i)
	{
		for (size_t	j = 0; j < rows; ++j)
		{
			diff[j].first = max[j].first - a[j*columns + i].first;
			diff[j].second = max[j].second - a[j*columns + i].second;
		}

		Meteor_funcRELU(diff, rp, max, rows);
		//funcSelectBitShares(maxPrime, dmpIndexShares, rp, temp, rows, columns, i);

		//for (size_t i = 0; i < size; ++i)
		//	maxPrime[i] = temp[i];

		for (size_t	j = 0; j < rows; ++j)
		{
			max[j].first = max[j].first + a[j*columns + i].first;
			max[j].second = max[j].second + a[j*columns + i].second;
		}
	}
}




/******************************** Test ********************************/


void testMeteorPC(size_t size, size_t iter)
{
	cout << "fuck off PC" << endl;
	MEVectorSmallType a(size*BIT_SIZE, make_pair(2, make_pair(0,0))), beta(size, make_pair(0,make_pair(0,0)));
	vector<smallType> betaPrime(size);
	vector<myType> r(size, 1);
	for(int runs = 0; runs < iter; runs++){
		Meteor_funcPrivateCompare(a, r, beta, betaPrime, size);
	}


}


void testMeteorfuncCrunchMultiply(size_t size, size_t iter)
{
	cout << "fuck off" << endl;
	MEVectorSmallType a(size*BIT_SIZE, make_pair(1, make_pair(0,0)));
	vector<smallType> betaPrime(size, 0);
	for(int runs = 0; runs < iter; runs++){
		Meteor_funcCrunchMultiply(a, betaPrime, size);
	}
}

void testThunderNMult(size_t size, size_t N,  size_t iter)
{
	cout << "fuck off N Mult" << endl;
	MEVectorType a(size*N, make_pair(1, make_pair(0,0)));
	for(int runs = 0; runs < iter; runs++){
		Thunder_funcNMultGate(a, size, N);
	}
}



void testMeteorNeighborMultly(size_t size, size_t iter)
{

/******************************** TODO ****************************************/	
	MEVectorSmallType a(size);
	for(int i = 0; i < size; i++)
	{
		a[i]=make_pair(i, make_pair(0,0));
	}
	//MEVectorSmallType b(size, make_pair(1, make_pair(0,0)) );
	MEVectorSmallType c(size/4);

	for (int runs = 0; runs < iter; ++runs)
		Meteor_funcMultiplyNeighbors(a, c, size);
}

void testMeteorBitProduct(size_t size, size_t iter)
{

/******************************** TODO ****************************************/	
	MEVectorSmallType a(size, make_pair(0, make_pair(0,1)) );
	MEVectorSmallType b(size, make_pair(1, make_pair(0,0)) );
	MEVectorSmallType c(size);

	for (int runs = 0; runs < iter; ++runs)
		Meteor_funcDotProductBits(a, b, c, size);
}

void testMeteorSmallDotProduct(size_t size, size_t iter)
{

/******************************** TODO ****************************************/	
	MEVectorSmallType a(size, make_pair(1, make_pair(0,0)) );
	MEVectorSmallType b(size, make_pair(2, make_pair(0,0)) );
	MEVectorSmallType c(size);

	for (int runs = 0; runs < iter; ++runs)
		Meteor_funcDotProduct(a, b, c, size);
}


void testMeteorDotProduct(size_t size, size_t iter)
{

/******************************** TODO ****************************************/	
	MEVectorType a(size, make_pair(1, make_pair(0,0)) );
	MEVectorType b(size, make_pair(2, make_pair(0,0)) );
	MEVectorType c(size);

	for (int runs = 0; runs < iter; ++runs)
		Meteor_funcDotProduct(a, b, c, size, false, FLOAT_PRECISION);
}

void testMeteorMatMul(size_t rows, size_t common_dim, size_t columns, size_t iter)
{

/******************************** TODO ****************************************/	
	MEVectorType a(rows*common_dim, make_pair(1, make_pair(0,0)) );
	MEVectorType b(common_dim*columns, make_pair(2, make_pair(0,0)) );
	MEVectorType c(rows*columns);

	for (int runs = 0; runs < iter; ++runs)
		Meteor_funcMatMul(a, b, c, rows, common_dim, columns, 0, 0, FLOAT_PRECISION);
}


void testMeteorRelu(size_t size, size_t iter)
{
	MEVectorType a(size, make_pair(0,make_pair(0,0)));
	MEVectorSmallType reluPrime(size);
	MEVectorType b(size);

	for (int runs = 0; runs < iter; ++runs)
	{
		Meteor_funcRELU(a, reluPrime, b, size);
		cout << "Meteor relu " << runs << " end" << endl;
	}
}



void testMeteorRELUPrime(size_t size, size_t iter)
{
	MEVectorType a(size, make_pair(1, make_pair(0,0)));
	MEVectorSmallType b(size);

	for(int runs = 0; runs < iter; runs++)
	{
		Meteor_funcRELUPrime(a, b, size);
	}
}

// (24, 24, 20, 2, 2, MINI_BATCH_SIZE, NUM_ITERATIONS);

void testMeteorTrainAvepool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter)
{
	testMeteorPredictAvepool(ih, iw, Din, f, S, B, iter);
	testMeteorBackAvepool(ih, iw, Din, f, S, B, iter);
}

void testMeteorBackAvepool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter)
{

	size_t ow 		= (((iw-f)/S)+1);
	size_t oh		= (((ih-f)/S)+1);
	size_t F 	= f*f;

	MEVectorType d(iw*ih*Din*B);
	MEVectorType a(ow*oh*Din*B, make_pair(1, make_pair(0,0)));
	//MEVectorSmallType c(iw*ih*Din*B);
	// MEVectorType temp1(ow*oh*Din*B*f*f);
	// size_t sizeBeta = iw;
	// size_t sizeD 	= sizeBeta*ih;
	// size_t sizeB 	= sizeD*Din;
	// size_t counter 	= 0;

	//RSSVectorSmallType temp1(iw*ih*Din*B);	//Contains maxPrime reordered
	MEVectorType temp2(iw*ih*Din*B);		//Contains Delta reordered
	{
		size_t sizeY 	= iw;
		size_t sizeD 	= sizeY*ih;
		size_t sizeB 	= sizeD*Din;
		size_t counter1 = 0;
		size_t counter2 = 0;

		for (int b = 0; b < B; ++b)
			for (size_t r = 0; r < Din; ++r)
				for (int y = 0; y < oh; ++y)
					for (int x = 0; x < ow; ++x)//onepoint responding a lot f*f
					{
						for (int q = 0; q < f; ++q)
						{
							for (int p = 0; p < f; ++p)
							{
								// temp1[b*sizeB + r*sizeD + 
								// 	(y*S + q)*sizeY + (x*S + p)] = 
								// maxPrime[counter1++];
								temp2[b*sizeB + r*sizeD + 
									(y*S + q)*sizeY + (x*S + p)] = 
								a[counter2];
								temp2[b*sizeB + r*sizeD + 
									(y*S + q)*sizeY + (x*S + p)].first = 
								a[counter2].first/F; 
								temp2[b*sizeB + r*sizeD + 
									(y*S + q)*sizeY + (x*S + p)].second.first = 
								a[counter2].second.first/F;
								temp2[b*sizeB + r*sizeD + 
									(y*S + q)*sizeY + (x*S + p)].second.second = 
								a[counter2].second.second/F;
							}
						}
						counter2++;//means one point' miss function is deltas,which is include f*f
					}
		Meteor_funcAveBackShares(temp2, d, iw*ih*Din*B);		
	}

	// for (int runs = 0; runs < iter; ++runs)
	// {
	// 	counter = 0;
	// 	for (int b = 0; b < B; ++b)
	// 		for (size_t r = 0; r < Din; ++r)
	// 			for (size_t beta = 0; beta < ih-f+1; beta+=S) 
	// 				for (size_t alpha = 0; alpha < iw-f+1; alpha+=S)
	// 					for (int q = 0; q < f; ++q)
	// 						for (int p = 0; p < f; ++p)
	// 						{
	// 							temp1[counter].first = 
	// 								a[b*sizeB + r*sizeD + 
	// 									(beta + q)*sizeBeta + (alpha + p)].first;
	// 							temp1[counter].second.first = 
	// 								a[b*sizeB + r*sizeD + 
	// 									(beta + q)*sizeBeta + (alpha + p)].second.first;
	// 							temp1[counter++].second.second = 
	// 								a[b*sizeB + r*sizeD + 
	// 									(beta + q)*sizeBeta + (alpha + p)].second.second;
	// 						}
	// 	//Pooling operation
	// 	Meteor_funcAvepool(temp1, b, ow*oh*Din*B, f*f);
	// }
}



void testMeteorPredictAvepool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter)
{

	size_t ow 		= (((iw-f)/S)+1);
	size_t oh		= (((ih-f)/S)+1);

	MEVectorType a(iw*ih*Din*B, make_pair(1, make_pair(0,0)));
	MEVectorType b(ow*oh*Din*B);
	//MEVectorSmallType c(iw*ih*Din*B);
	MEVectorType temp1(ow*oh*Din*B*f*f);
	size_t sizeBeta = iw;
	size_t sizeD 	= sizeBeta*ih;
	size_t sizeB 	= sizeD*Din;
	size_t counter 	= 0;

	for (int runs = 0; runs < iter; ++runs)
	{
		counter = 0;
		for (int b = 0; b < B; ++b)
			for (size_t r = 0; r < Din; ++r)
				for (size_t beta = 0; beta < ih-f+1; beta+=S) 
					for (size_t alpha = 0; alpha < iw-f+1; alpha+=S)
						for (int q = 0; q < f; ++q)
							for (int p = 0; p < f; ++p)
							{
								temp1[counter].first = 
									a[b*sizeB + r*sizeD + 
										(beta + q)*sizeBeta + (alpha + p)].first;
								temp1[counter].second.first = 
									a[b*sizeB + r*sizeD + 
										(beta + q)*sizeBeta + (alpha + p)].second.first;
								temp1[counter++].second.second = 
									a[b*sizeB + r*sizeD + 
										(beta + q)*sizeBeta + (alpha + p)].second.second;
							}
		//Pooling operation
		Meteor_funcAvepool(temp1, b, ow*oh*Din*B, f*f);
	}
}


void testMeteorMaxpool(size_t ih, size_t iw, size_t Din, size_t f, size_t S, size_t B, size_t iter)
{
	size_t ow 		= (((iw-f)/S)+1);
	size_t oh		= (((ih-f)/S)+1);

	MEVectorType a(iw*ih*Din*B, make_pair(1, make_pair(0,0)));
	MEVectorType b(ow*oh*Din*B);
	MEVectorSmallType c(iw*ih*Din*B);
	MEVectorType temp1(ow*oh*Din*B*f*f);
	size_t sizeBeta = iw;
	size_t sizeD 	= sizeBeta*ih;
	size_t sizeB 	= sizeD*Din;
	size_t counter 	= 0;

	for (int runs = 0; runs < iter; ++runs)
	{
		counter = 0;
		for (int b = 0; b < B; ++b)
			for (size_t r = 0; r < Din; ++r)
				for (size_t beta = 0; beta < ih-f+1; beta+=S) 
					for (size_t alpha = 0; alpha < iw-f+1; alpha+=S)
						for (int q = 0; q < f; ++q)
							for (int p = 0; p < f; ++p)
							{
								temp1[counter].first = 
								a[b*sizeB + r*sizeD + (beta + q)*sizeBeta + (alpha + p)].first;
								temp1[counter].second.first = 
								a[b*sizeB + r*sizeD + (beta + q)*sizeBeta + (alpha + p)].second.first;
								temp1[counter].second.second = 
								a[b*sizeB + r*sizeD + (beta + q)*sizeBeta + (alpha + p)].second.second;
								counter++;
							}
		//Pooling operation
		Meteor_funcMaxpool(temp1, b, ow*oh*Din*B, f*f);
	}
}

void testMeteorConvolution(size_t iw, size_t ih, size_t Din, size_t Dout, size_t f, size_t S, size_t P, size_t B, size_t iter)
{
	size_t ow 		= (((iw-f+2*P)/S)+1);
	size_t oh		= (((ih-f+2*P)/S)+1);
	size_t tempSize = ow*oh;

	MEVectorType a(iw*ih*Din*B, make_pair(0, make_pair(0,0)));
	MEVectorType b(f*f*Din*Dout, make_pair(0, make_pair(0,0)));
	MEVectorType ans(ow*oh*Dout*B, make_pair(0, make_pair(0,0)));
	MEVectorType c(Dout, make_pair(0, make_pair(0,0)));

	for (int runs = 0; runs < iter; ++runs)
	{
	// 	//Reshape activations
	 	MEVectorType temp1((iw+2*P)*(ih+2*P)*Din*B, make_pair(0, make_pair(0,0)));
	 	zeroPad(a, temp1, iw, ih, P, Din, B);

	 	//Reshape for convolution
	 	MEVectorType temp2((f*f*Din) * (ow * oh * B));
	 	//convToMult(temp1, temp2, (iw+2*P), (ih+2*P), f, Din, S, B);
		{
		size_t loc_input, loc_output;
		for (size_t i = 0; i < B; ++i)
			for (size_t j = 0; j < oh; j++)
				for (size_t k = 0; k < ow; k++)
				{
					loc_output = (i*ow*oh + j*ow + k);
					for (size_t l = 0; l < Din; ++l) 
					{
						loc_input = i*(iw+2*P)*(ih+2*P)*Din + l*(iw+2*P)*(ih+2*P) + j*S*(iw+2*P) + k*S;
						for (size_t a = 0; a < f; ++a)			//filter height
							for (size_t b = 0; b < f; ++b){		//filter width
								temp2[(l*f*f + a*f + b)*ow*oh*B + loc_output].first = temp1[loc_input + a*(iw+2*P) + b].first;
								temp2[(l*f*f + a*f + b)*ow*oh*B + loc_output].second = temp1[loc_input + a*(iw+2*P) + b].second;
							}
					}
				}
		}
		MEVectorType temp3(Dout * (ow*oh*B));
		Meteor_funcMatMul(b, temp2, temp3, Dout, (f*f*Din), (ow*oh*B), 0, 1, FLOAT_PRECISION);


	// 	//Add biases and meta-transpose
		for (size_t i = 0; i < B; ++i)
	 		for (size_t j = 0; j < Dout; ++j) 
	 			for (size_t k = 0; k < tempSize; ++k){
	 				ans[i*Dout*tempSize + j*tempSize + k].first = temp3[j*B*tempSize + i*tempSize + k].first + c[j].first;	
	 				ans[i*Dout*tempSize + j*tempSize + k].second = temp3[j*B*tempSize + i*tempSize + k].second + c[j].second;	
				 }	
	 }
}

void test_MeteorBatchNorm(size_t numBatches, size_t inputSize, size_t iter)
{
	size_t B = numBatches;
	size_t m = inputSize;

	MEVectorType gamma(numBatches, make_pair(0, make_pair(0,0))), g_repeat(B*m), input(B*m, make_pair(0, make_pair(0,0))), activations(B*m, make_pair(0, make_pair(0,0))), beta(B, make_pair(0, make_pair(0,0)));

	for(int runs = 0; runs < iter; runs++){	
		for (int i = 0; i < B; ++i){
			for (int j = 0; j < m; ++j){
				g_repeat[i*m+j] = gamma[i];
			}
		}

		Meteor_funcDotProduct(g_repeat, input, activations, B*m, true, FLOAT_PRECISION);

		for (int i = 0; i < B; ++i){
			for (int j = 0; j < m; ++j){
				activations[i*m+j].first = activations[i*m+j].first + beta[i].first;
				activations[i*m+j].second = activations[i*m+j].second + beta[i].second;
			}
		}
	}

}

//Finally,I use a quqiao  way to acieve this process,(i use a Product instead of a adding sharing) and it'sOK.
void Meteor_funcAveBackShares(const MEVectorType &a, 
								MEVectorType &selected, size_t size)
{
	log_print("funcSelectShares");

	//MEVectorType c(size), bXORc(size);
	MEVectorType m_c(size);
	vector<myType> Random_Share(size, 0);
	// vector<smallType> reconst_b(size);
	PrecomputeObject.getSelectBitShares(m_c, size);
// mo fang related process.
	// if(partyNum == PARTY_A){
	// 	thread *threads = new thread[4];
	// 	threads[0] = thread(sendVector<myType>, ref(Random_Share), prevParty(partyNum), size);
	// 	threads[1] = thread(receiveVector<myType>, ref(Random_Share), nextParty(partyNum), size);
	// 	threads[2] = thread(receiveVector<myType>, ref(Random_Share), prevParty(partyNum), size);
	// 	threads[3] = thread(sendVector<myType>, ref(Random_Share), nextParty(partyNum), size);
		
	// 	for (int i = 0; i < 4; i++)
	// 		threads[i].join();
	// 		delete[] threads;
	// }
	//thread *threads = new thread[3];

	// threads[0] = thread(sendVector<myType>, ref(m_c), prevParty(partyNum), size);
	// threads[1] = thread(sendVector<myType>, ref(m_c), prevParty(partyNum), size);
	// threads[2] = thread(receiveVector<myType>, ref(m_c), nextParty(partyNum), size);
	// threads[3] = thread(receiveVector<myType>, ref(m_c), nextParty(partyNum), size);
		
	// for (int i = 0; i < 3; i++)
	// 	threads[i].join();
	// delete[] threads; 

	if (partyNum == PARTY_A)
		for (int i = 0; i < size; ++i)
			//if (reconst_b[i] == 0)
			{
				m_c[i].first = 1;
				m_c[i].second=make_pair(0,0);
			}

	if (partyNum == PARTY_B)
		for (int i = 0; i < size; ++i)
			//if (reconst_b[i] == 0)
			{
				m_c[i].first = 1;
				m_c[i].second=make_pair(0,0);
			}

	if (partyNum == PARTY_C)
		for (int i = 0; i < size; ++i)
			//if (reconst_b[i] == 0)
			{
				m_c[i].first = 1;
				m_c[i].second=make_pair(0,0);
			}

	
	// for (int i = 0; i < size; ++i)
	// 		//if (reconst_b[i] == 0)
	// 		{
	// 			a[i].first = select[i].first;
	// 			a[i].second = select[i].second;
	// 		}
	Meteor_funcDotProduct(a, m_c, selected, size, false, 0);
}

//Chunk wise maximum of a vector of size rows*columns and maximum is caclulated of every 
//column number of elements. max is a vector of size rows, of rows*columns*columns; 
void Meteor_funcAvepool(MEVectorType &a, MEVectorType &ave,  size_t rows, size_t columns)
{
	log_print("funcAvepool");
	assert(columns < 256 && "Pooling size has to be smaller than 8-bits");

	size_t size = rows*columns;
	MEVectorType diff(rows);
	MEVectorType rp(rows);
	// RSSVectorSmallType rp(rows), dmpIndexShares(columns*size), temp(size);
	// vector<smallType> dmpTemp(columns*size, 0);

	// for (int loopCounter = 0; loopCounter < columns; ++loopCounter)
	// 	for (size_t i = 0; i < rows; ++i)
	// 		dmpTemp[loopCounter*rows*columns + i*columns + loopCounter] = 1;
	// funcGetShares(dmpIndexShares, dmpTemp);
	for (size_t	j = 0; j < rows; ++j)
		diff[j] = a[j*columns];
	//Get the sum
	for (size_t	j = 0; j < rows; ++j)
	{
		for (size_t i = 1; i < columns; ++i)
		{
			diff[j] = diff[j]+a[j*columns + i];
		}
		ave[j] = diff[j];
	}
	//get the average
	for (size_t	j = 0; j < rows; ++j)
	{
		diff[j].first=diff[j].first/columns;
		diff[j].second.first=diff[j].second.first/columns;
		diff[j].second.first=diff[j].second.second/columns;
		ave[j] = diff[j];
	}

	// for (size_t i = 0; i < size; ++i)
	// 	maxPrime[i] = dmpIndexShares[i];
	// for (size_t	j = 0; j < rows; ++j)
	// 	diff[j] = a[j*columns];
	// for (size_t	j = 0; j < rows; ++j)
	// {
	// 	for (size_t i = 1; i < columns; ++i)
	// 	{
	// 		diff[j] = diff[j]+a[j*columns + i];
	// 		// diff[j].first=diff[j].first/columns;
	// 		// diff[j].second=diff[j].second/columns;
	// 	}
	// 	max[j] = diff[j];
	// }



	// for (size_t i = 0; i < rows; ++i)
	// 	max[i] = a[i*columns];

	// for (size_t i = 1; i < columns; ++i)//Perfect,which can make the max from f*f!
	// {
	// 	for (size_t	j = 0; j < rows; ++j)
	// 		diff[j] = max[j] - a[j*columns + i];

	// 	funcRELU(diff, rp, max, rows);//max=Relu(diff),rp=Relu'(diff)
	// 	// funcSelectBitShares(maxPrime, dmpIndexShares, rp, temp, rows, columns, i);//i don't understand,and i know what it want do,but,canot arrive final result.

	// 	// for (size_t i = 0; i < size; ++i)
	// 	// 	maxPrime[i] = temp[i];

	// 	for (size_t	j = 0; j < rows; ++j)//if max[i]>max(i+1),max(j)=max[i];else,max(i+1);
	// 		max[j] = max[j] + a[j*columns + i];
	// }
}

