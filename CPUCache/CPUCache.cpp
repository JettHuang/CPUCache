// \brief
//		CPU Cache And Performence.
// URL: https://blog.csdn.net/cool_way/article/details/41855595
//      http://igoro.com/archive/gallery-of-processor-cache-effects/
//

#include <cstdio>
#include <cstdlib>
#include <Windows.h>

#include "WindowsTime.h"

// Memory accesses and performance
void Example_0()
{
	const int Count = 64 * 1024 * 1024;

	int *pArray1 = new int[Count + 16];
	int *pArray2 = new int[Count + 16];

	int *pAlignArray1 = (int*)((((int64_t)pArray1) + 64) &(~64));
	int *pAlignArray2 = (int*)((((int64_t)pArray2) + 64) &(~64));

	printf("pAlignArray1 Addr: 0x%p\n", pAlignArray1);
	printf("pAlignArray2 Addr: 0x%p\n", pAlignArray2);

	double Start, End;
	int Total = 0;
	// Loop 1
	Start = appSeconds();
	for (int k = 0; k < Count; k+=(16*4)) // 每次跨越4 * cache line(cacheline=64bytes)
	{
		// 访问同一个cache line
		Total += pAlignArray1[k];
		Total += pAlignArray1[k+1];
		Total += pAlignArray1[k+2];
		Total += pAlignArray1[k+3];
	}

	End = appSeconds();
	printf("Loop 1 Cycles: %.9f, Total=%d\n", (float)(End - Start), Total);

	// Loop 2
	Start = appSeconds();
	for (int k = 0; k < Count; k += (16*4)) // 每次跨越4 * cache line(cacheline=64bytes)
	{
		// 访问不同的cache line
		Total += pAlignArray2[k];
		Total += pAlignArray2[k + 16];
		Total += pAlignArray2[k + 32];
		Total += pAlignArray2[k + 48];
	}

	End = appSeconds();
	printf("Loop 2 Cycles: %.9f, Total=%d \n", (float)(End - Start), Total);

	delete[] pArray1;
	delete[] pArray2;
}

// Memory accesses and performance
void Example_1()
{
	const int Count = 64 * 1024 * 1024;

	int *pArray1 = new int[Count];
	int *pArray2 = new int[Count];
	printf("pAddray1 Addr: 0x%p\n", pArray1);
	printf("pAddray2 Addr: 0x%p\n", pArray2);

	int64_t Start, End;
	// Loop 1
	Start = appCycles();
	for (int k = 0; k < Count; k++)
	{
		pArray1[k] *= 3;
	}
	End = appCycles();
	printf("Loop 1 Cycles: %lld\n", (End - Start));

	// Loop 2
	Start = appCycles();
	for (int k = 0; k < Count; k += 16)
	{
		pArray2[k] *= 3;
	}
	End = appCycles();
	printf("Loop 2 Cycles: %lld\n", (End - Start));

	delete[] pArray1;
	delete[] pArray2;
}

// Impact of cache lines
void Example_2()
{
	const int ArrayCount = 64 * 1024 * 1024;

	int *pArray1 = new int[ArrayCount];
	printf("pAddray1 Addr: 0x%p\n", pArray1);

	const int nStepCount = 8;
	int Step = 1;

	for (int Step = 1, Index = 1; Index <= nStepCount; Step <<= 1, Index++)
	{
		int64_t Start, End;
		int LoopCount = 0;
		Start = appCycles();
		for (int k = 0; k < ArrayCount; k += Step)
		{
			pArray1[k] *= 3;
			LoopCount++;
		}
		End = appCycles();
		printf("Step=%d, Loops=%d, Cycles: %lld\n", Step, LoopCount, (End - Start));
	}

	delete[] pArray1;
}

// L1 and L2 cache sizes
void Example_3()
{
	const int ArrayCount = 64 * 1024 * 1024;

	int *pArray1 = new int[ArrayCount];
	printf("pAddray1 Addr: 0x%p\n", pArray1);

	const int Steps = 64 * 1024 * 1024;
	for (int Size = 64; Size <= ArrayCount; Size <<= 1)
	{
		int64_t Start, End;

		Start = appCycles();
		for (int k = 0; k < Steps; k++)
		{
			pArray1[(k * 16) % Size]++;
		}
		End = appCycles();

		printf("ArraySize=%9d, Loops=%9d, Cycles: %lld\n", Size, Steps, (End - Start));
	}

	delete[] pArray1;
}

// Instruction-level parallelism
void Example_4()
{
	const int Steps = 256 * 1024 * 1024;
	int a[2] = { 0 };

	int64_t Start, End;
	Start = appCycles();
	for (int k = 0; k < Steps; k++)
	{
		a[0]++;
		a[0]++;
	}
	End = appCycles();
	printf("Loops=%9d, Cycles: %lld\n", Steps, (End - Start));

	Start = appCycles();
	for (int k = 0; k < Steps; k++)
	{
		a[0]++;
		a[1]++;
	}
	End = appCycles();
	printf("Loops=%9d, Cycles: %lld\n", Steps, (End - Start));

}

// Cache associativity
// 以步长为K访问数组
// 8-way 32KB Cache1,  8-way 32KB Cache2, 12-way 6M Cache3
static void VisitArray(char Array[], int ArraySize, int K)
{
	const int Count = 64 * 1024 * 1024;

	int64_t Start, End;
	Start = appCycles();
	for (int k = 0, i = 0; k < Count; k++)
	{
		Array[i]++;
		i += K;
		i %= ArraySize;
	}
	End = appCycles();
	printf("Loops=%9d, ArraySize=%9d, Step=%9d, Cycles: %lld\n", Count, ArraySize, K, (End - Start));
}

void Example_5()
{
	const int MaxSize = 64 * 1024 * 1024; // 64M
	char *pArray = new char[MaxSize];

	const int SizeArray[] = {
		32 * 1024,
		64 * 1024,
		128 * 1024,
		1024 * 1024,
		6 * 1024 * 1024,
		12 * 1024 * 1024
	};
	const int StepArray[] = {
		1,
		2,
		4,
		8,
		64,
		128,
		256,
		512
	};

	for (int m = 0; m < sizeof(SizeArray) / sizeof(SizeArray[0]); m++)
	{
		int Size = SizeArray[m];
		for (int n = 0; n < sizeof(StepArray) / sizeof(StepArray[0]); n++)
		{
			int K = StepArray[n];
			VisitArray(pArray, Size, K);
		}
	}

	delete pArray;
}

// Exmaple 6: False cache line sharing
static const int kThreadsNum = 4;
static int s_Counter[1024] = { 0 };

struct FThreadParameter
{
	int ThreadIndex;
	int VisitIndex;
};
struct FThreadResult
{
	int VisitIndex;
	int64_t Cycles;
};

static FThreadResult s_Results[kThreadsNum];

DWORD WINAPI Example6_ThreadProc(_In_ LPVOID lpParameter)
{
	FThreadParameter *Param = (FThreadParameter *)lpParameter;
	const int Index = Param->VisitIndex;
	const int ThreadIndex = Param->ThreadIndex;

	int64_t Start, End;
	Start = appCycles();
	for (int j = 0; j < 100000000; j++)
	{
		s_Counter[Index] = s_Counter[Index] + 3;
	}
	End = appCycles();

	s_Results[ThreadIndex].VisitIndex = Index;
	s_Results[ThreadIndex].Cycles = End - Start;

	return 0;
}

void Example_6_Case(const int *VisitIndex)
{
	HANDLE  hThreadArray[kThreadsNum];
	FThreadParameter ThreadParameters[kThreadsNum];


	for (int k = 0; k < kThreadsNum; k++)
	{
		ThreadParameters[k].ThreadIndex = k;
		ThreadParameters[k].VisitIndex = VisitIndex[k];

		hThreadArray[k] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			Example6_ThreadProc,    // thread function name
			&(ThreadParameters[k]), // argument to thread function 
			CREATE_SUSPENDED,       // use default creation flags 
			NULL);					// returns the thread identifier 
	} // end for
	for (int k = 0; k < kThreadsNum; k++)
	{
		ResumeThread(hThreadArray[k]);
	}

	// Wait until all threads have terminated.
	WaitForMultipleObjects(kThreadsNum, hThreadArray, TRUE, INFINITE);

	// display the result:
	for (int k = 0; k < kThreadsNum; k++)
	{
		printf("Thread %d: VisitPos=%4d, Cycles=%lld\n", k, s_Results[k].VisitIndex, s_Results[k].Cycles);
	}
}

void Example_6()
{
	const int VisitIndex0[] = { 0, 1, 2, 3 };
	const int VisitIndex1[] = { 16, 32, 48, 64 };

	printf("Case 0: \n");
	Example_6_Case(VisitIndex0);

	printf("Case 1: \n");
	Example_6_Case(VisitIndex1);
}

//Hardware complexities
void Example_7()
{
	int A=0, B=0, C=0, D=0, E=0, F=0, G=0;
	int64_t Start, End;
	

	// Case 0:
	Start = appCycles();
	{
		for (int i = 0; i < 200000000; i++)
		{
			A++; B++; C++; D++;
		}
	}
	End = appCycles();
	printf("A++; B++; C++; D++; Cycles: %lld\n", (End - Start));

	// Case 1:
	Start = appCycles();
	{
		for (int i = 0; i < 200000000; i++)
		{
			A++; C++; E++; G++;
		}
	}
	End = appCycles();
	printf("A++; C++; E++; G++; Cycles: %lld\n", (End - Start));

	// Case 2:
	Start = appCycles();
	{
		for (int i = 0; i < 200000000; i++)
		{
			A++; C++;
		}
	}
	End = appCycles();
	printf("A++; C++;           Cycles: %lld\n", (End - Start));
}

void main()
{
	appInitTiming();

	Example_0();
	//Example_1();
	//Example_2();
	//Example_3();
	//Example_4();
	//Example_5();
	//Example_6();
	//Example_7();

	system("pause");
}
