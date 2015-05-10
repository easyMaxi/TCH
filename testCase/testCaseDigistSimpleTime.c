const char *testCaseDigestSimpleTimeDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_DigestInit", "C_Digest",  NULL};

int
digestSimpleTime(CK_SLOT_ID slotID, CK_MECHANISM_PTR mechanism, CK_SESSION_HANDLE hSession, CK_BYTE_PTR data, CK_BYTE_PTR *dataDigest, CK_ULONG_PTR dataDigestLen)
{
	CK_RV rv;
	
	rv = pFunctionList->C_DigestInit(hSession, mechanism);
	if(rv != CKR_OK)
	{
		printf(" C_DigestInit failed");
		returnTestCase(rv, 0);
	}
	
	//Digest with NULL_PTR
	rv = pFunctionList->C_Digest(hSession, data, strlen( (char*) data), NULL_PTR, dataDigestLen);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	
	free(*dataDigest);
	*dataDigest = malloc((*dataDigestLen) * sizeof(CK_BYTE));
	
	//Digest 
	rv = pFunctionList->C_Digest(hSession, data, strlen( (char*) data), *dataDigest, dataDigestLen);
	if(rv != CKR_OK)
	{
		printf(" C_Digest failed");
		returnTestCase(rv, 0);
	}

	return 0;
}

int
testCaseDigestSimpleTime()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_MECHANISM mechanism = {
		0, NULL_PTR, 0
	};
	CK_BYTE_PTR data = NULL_PTR;
	CK_BYTE_PTR dataDigest = NULL_PTR;
	CK_BYTE_PTR dataDigest2 = NULL_PTR;
	CK_ULONG dataDigestLen = 0;
	CK_ULONG dataDigestLen2 = 0;
	clock_t start;
	
	int i,j,k,l, errorcount=0;
		
	printf("Testing case: DigestSimpleTime ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseDigestSimpleTimeDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	//initialize
	printf(" call Initialize ...");
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	//GetSlotList -> Tokenlist
	findActiveTokens(&pTokenList, &ulTokenCount);
	
	for(i=0;i<ulTokenCount;i++)
	{
		slotID = pTokenList[i];
		
		//OpenSession with CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		for(j=0;j<pMechaInfoListLen;j++)//each mechanism
		{
			if(!(pMechaInfoList[j].info.flags & CKF_DIGEST)) continue; //that is capable of digesting
			mechanism.mechanism = pMechaInfoList[j].type;
			
			
			k = searchEleName(mechanism.mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
			if(k != -1)	printf(" Digest %s two times ... \n", MECHANISM_LIST[k].eleName);
			else printf(" Digest(mecha = %lu) two times ... \n", mechanism.mechanism);
	
			generateSampleData(16, &data);
			digestSimpleTime(slotID, &mechanism, hSession, data, &dataDigest, &dataDigestLen);
			digestSimpleTime(slotID, &mechanism, hSession, data, &dataDigest2, &dataDigestLen2);

			
			printf(" compare digests\n");
			errorcount=0;
			if(dataDigestLen2 == dataDigestLen)
			{
				for (l = 0; l < dataDigestLen2; l++) {
					if(dataDigest2[l] != dataDigest[l])
					{
						errorcount++;
						break;
					}
				}
			}
			else
			{
				printf("  Digestlengths are different\n");
			}
			if(errorcount==0) printf("  Digests are congruent\n");
			else printf("  Digests are different\n");			
			
			
			generateSampleData(16, &data);
			start = time(NULL);
			digestSimpleTime(slotID, &mechanism, hSession, data, &dataDigest, &dataDigestLen);
			printf(" could digest %zd CK_BYTEs in ca. %f sec \n", strlen((char *) data), (double)(time(NULL) - start));
			
			generateSampleData(1048576, &data);
			start = time(NULL);
			digestSimpleTime(slotID, &mechanism, hSession, data, &dataDigest, &dataDigestLen);
			printf(" could digest %zd CK_BYTEs in ca. %f sec \n", strlen((char *) data), (double)(time(NULL) - start));
			
			generateSampleData(33554432, &data);
			start = time(NULL);
			digestSimpleTime(slotID, &mechanism, hSession, data, &dataDigest, &dataDigestLen);
			printf(" could digest %zd CK_BYTEs in ca. %f sec \n", strlen((char *) data), (double)(time(NULL) - start));

		}		
	}
	
	//finalize
	printf(" call Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	free(pTokenList);
	free(dataDigest);
	free(dataDigest2);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
