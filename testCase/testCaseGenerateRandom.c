const char *testCaseGenerateRandomDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_GenerateRandom",  NULL};

#define RAND_64 64
#define RAND_1024 1024
#define RAND_8192 8192
#define RAND_ROUNDS 1000

int
testCaseGenerateRandom()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_TOKEN_INFO info;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_BYTE_PTR seed = NULL_PTR;
	CK_BYTE randBytes64[RAND_64];
	CK_BYTE randBytes1024[RAND_1024];
	CK_BYTE randBytes8192[RAND_8192];
	struct timeval start, end, diff;
	int i;
		
	printf("Testing case: GenerateRandom ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSlotEventsDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	initToken();
	
	//GetSlotList -> Tokenlist
	findActiveTokens(&pTokenList, &ulTokenCount);
	
	for(i=0;i<ulTokenCount;i++)
	{
		slotID = pTokenList[i];
		
		
		//GetTokenInfo
		printf(" call GetSlotInfo on slot %lu ...", slotID);
		rv = pFunctionList->C_GetTokenInfo(slotID, &info);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//check CKF_RNG flag
		if(info.flags & CKF_RNG) printf(" CKF_RNG is set\n");
		else printf(" CKF_RNG is not set\n");
		
		//OpenSession withCKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//C_SeedRandom
		rv = checkFunctionImplemented("C_SeedRandom");
		if(rv != 1)printf(" skip C_SeedRandom(NOT_SUPPORTED)");
		else
		{
			generateSampleData(64, &seed);
			printf(" call C_SeedRandom on slot %lu ...", slotID);
			rv = pFunctionList->C_SeedRandom(hSession, seed, 64);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");
			free(seed);
		}
		
		//C_GenerateRandom 64
		printf(" call C_GenerateRandom(%d Bytes) %d times on slot %lu ...\n", RAND_64, RAND_ROUNDS, slotID);
		int j,k;

		gettimeofday(&start, NULL); //dont use clock() because the time on the token is not measured
		for(j=0;j<RAND_ROUNDS;j++)
		{
			rv = pFunctionList->C_GenerateRandom(hSession, randBytes64, RAND_64);
			if(rv != CKR_OK) returnTestCase(rv, 0);						
		}
		gettimeofday(&end, NULL);
		timeval_subtract(&diff, &end, &start);
		printf("  Could call C_GenerateRandom %d times with %d bytes in %ld.%06ld sec. \n", RAND_ROUNDS, RAND_64, diff.tv_sec, diff.tv_usec);
		
		printf("  append data to file rand_64.txt.\n");
		FILE *file = fopen("rand_64.txt", "a");
		if (file == NULL)
		{
			printf("   Error opening/creating file!\n");
		}
		//writing to file
		for(j=0;j<RAND_ROUNDS;j++)
		{
			if(file != NULL)
			{
				fprintf(file, "\n");
				for (k = 0; k < RAND_64; k++) {
					fprintf(file, "%.2x", randBytes64[k]);
				}
					
			}
		}
		fclose(file);
		printf(" done.\n");

	
		//C_GenerateRandom 1024
		printf(" call C_GenerateRandom(%d Bytes) %d times on slot %lu ...\n", RAND_1024, RAND_ROUNDS, slotID);

		gettimeofday(&start, NULL); //dont use clock() because the time on the token is not measured
		for(j=0;j<RAND_ROUNDS;j++)
		{
			rv = pFunctionList->C_GenerateRandom(hSession, randBytes1024, RAND_1024);
			if(rv != CKR_OK) returnTestCase(rv, 0);						
		}
		gettimeofday(&end, NULL);
		timeval_subtract(&diff, &end, &start);
		printf("  Could call C_GenerateRandom %d times with %d bytes in %ld.%06ld sec. \n", RAND_ROUNDS, RAND_1024, diff.tv_sec, diff.tv_usec);
		
		printf("  append data to file rand_1024.txt.\n");
		file = fopen("rand_1024.txt", "a");
		if (file == NULL)
		{
			printf("   Error opening/creating file!\n");
		}
		//writing to file
		for(j=0;j<RAND_ROUNDS;j++)
		{
			if(file != NULL)
			{
				fprintf(file, "\n");
				for (k = 0; k < RAND_1024; k++) {
					fprintf(file, "%.2x", randBytes1024[k]);
				}
					
			}
		}
		fclose(file);
		printf(" done.\n");
	
	
		//C_GenerateRandom 8192
		printf(" call C_GenerateRandom(%d Bytes) %d times on slot %lu ...\n", RAND_8192, RAND_ROUNDS, slotID);

		gettimeofday(&start, NULL); //dont use clock() because the time on the token is not measured
		for(j=0;j<RAND_ROUNDS;j++)
		{
			rv = pFunctionList->C_GenerateRandom(hSession, randBytes8192, RAND_8192);
			if(rv != CKR_OK) returnTestCase(rv, 0);						
		}
		gettimeofday(&end, NULL);
		timeval_subtract(&diff, &end, &start);
		printf("  Could call C_GenerateRandom %d times with %d bytes in %ld.%06ld sec. \n", RAND_ROUNDS, RAND_8192, diff.tv_sec, diff.tv_usec);
		
		printf("  append data to file rand_8192.txt.\n");
		file = fopen("rand_8192.txt", "a");
		if (file == NULL)
		{
			printf("   Error opening/creating file!\n");
		}
		//writing to file
		for(j=0;j<RAND_ROUNDS;j++)
		{
			if(file != NULL)
			{
				fprintf(file, "\n");
				for (k = 0; k < RAND_8192; k++) {
					fprintf(file, "%.2x", randBytes8192[k]);
				}
					
			}
		}
		fclose(file);
		printf(" done.\n");
	}
	
	finalizeToken();
	
	free(pTokenList);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
