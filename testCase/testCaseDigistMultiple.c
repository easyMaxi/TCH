const char *testCaseDigestMultipleDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_DigestInit", "C_DigestUpdate", "C_DigestFinal",  NULL};

int
digestMultiple(CK_SLOT_ID slotID, CK_MECHANISM_PTR mechanism, CK_SESSION_HANDLE hSession, CK_BYTE_PTR data, CK_BYTE_PTR *dataDigest, CK_ULONG_PTR dataDigestLen)
{
	int k;
	CK_RV rv;
	CK_ULONG dataHalf = strlen((char * )data) / 2;
	CK_BYTE_PTR dataFirstHalf;
	CK_BYTE_PTR dataSecondHalf;
	
	dataFirstHalf = data; //first half of the string
	dataSecondHalf = data + (dataHalf * sizeof(CK_BYTE)); //second half of the string	
	
	
	k = searchEleName(mechanism->mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
	
	rv = setDigestParams(mechanism);
	if(rv == 0)
	{

		if(k != -1)	printf(" Digest:%s on slot %lu ...\n", MECHANISM_LIST[k].eleName, slotID);
		else printf(" Digest(mecha = %lu) on slot %lu ... \n", mechanism->mechanism, slotID);
		
		//DigestInit
		printf("  call C_DigestInit on slot %lu ...", slotID);
		rv = pFunctionList->C_DigestInit(hSession, mechanism);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		
		dataDigest = malloc((*dataDigestLen) * sizeof(CK_BYTE));
		
		//C_DigestUpdate 1/2
		printf("  call C_DigestUpdate on slot %lu ...", slotID);
		rv = pFunctionList->C_DigestUpdate(hSession, dataFirstHalf, dataHalf);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//C_DigestUpdate 2/2
		printf("  call C_DigestUpdate on slot %lu ...", slotID);
		rv = pFunctionList->C_DigestUpdate(hSession, dataSecondHalf, dataHalf);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//C_DigestFinal
		printf("  call C_DigestFinal on slot %lu with NULL_PTR ...", slotID);
		rv = pFunctionList->C_DigestFinal(hSession, NULL_PTR, dataDigestLen);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		*dataDigest = malloc((*dataDigestLen) * sizeof(CK_BYTE));
		
		//C_DigestFinal
		printf("  call C_DigestFinal on slot %lu ...", slotID);
		rv = pFunctionList->C_DigestFinal(hSession, *dataDigest, dataDigestLen);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		int j;
		for (j = 0; j < (*dataDigestLen); j++) {
			printf("%.2x", (*dataDigest)[j]);
		}
		printf(" <- %s \n", data);		
	}
	else
	{
		if(k != -1)	printf(" skip Digesting(%s) because mecha is not yet implemented ...\n", MECHANISM_LIST[k].eleName);
		else printf(" skip Digesting(mecha = %lu) because mecha is not yet implemented ...\n", mechanism->mechanism);
	}

	
	return 0;
}

int
testCaseDigestMultiple()
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
	CK_BYTE_PTR data;
	generateSampleData(64, &data);
	CK_BYTE_PTR dataDigest = NULL_PTR;
	CK_ULONG dataDigestLen = 0;
	
	int i,j;
		
	printf("Testing case: DigestMultiple ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseDigestMultipleDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	//initialize
	initToken();
	
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
			
			digestMultiple(slotID, &mechanism, hSession, data, &dataDigest, &dataDigestLen);
		}		
	}
	
	//finalize
	finalizeToken();
	
	free(pTokenList);
	free(dataDigest);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
