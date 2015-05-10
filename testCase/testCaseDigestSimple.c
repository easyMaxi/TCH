const char *testCaseDigestSimpleDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_DigestInit", "C_Digest",  NULL};

int
digestSimple(CK_SLOT_ID slotID, CK_MECHANISM_PTR mechanism, CK_SESSION_HANDLE hSession, CK_BYTE_PTR data, CK_BYTE_PTR dataDigest, CK_ULONG_PTR dataDigestLen)
{
	int k;
	CK_RV rv;
	CK_OBJECT_HANDLE hKey = 0;
	CK_MECHANISM keyMechanism = {
		CKM_DES_KEY_GEN, NULL_PTR, 0
	};
	
	
	k = searchEleName(mechanism->mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
	
	rv = setDigestParams(mechanism);
	if(rv == 0)
	{

		if(k != -1)	printf(" Digest:%s on slot %lu ...\n", MECHANISM_LIST[k].eleName, slotID);
		else printf(" Digest(mecha = %lu) on slot %lu ...\n", mechanism->mechanism, slotID);
		
		//DigestInit
		printf("  call C_DigestInit on slot %lu ...", slotID);
		rv = pFunctionList->C_DigestInit(hSession, mechanism);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//Digest with NULL_PTR
		printf("  call C_Digest on slot %lu with NULL_PTR ...", slotID);
		rv = pFunctionList->C_Digest(hSession, data, strlen( (char*) data), NULL_PTR, dataDigestLen);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		dataDigest = malloc((*dataDigestLen) * sizeof(CK_BYTE));
		
		//Digest
		printf("  call C_Digest on slot %lu ...", slotID);
		rv = pFunctionList->C_Digest(hSession, data, strlen( (char*) data), dataDigest, dataDigestLen);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		int j;
		for (j = 0; j < (*dataDigestLen); j++) {
			printf("%.2x", dataDigest[j]);
		}
		printf(" <- %s \n", data);		
		
		
		/* Digist Key CKM_DES_KEY_GEN */
		if(checkFunctionImplemented("C_DigestKey"))
		{
			printf("   Try to digest a Key from CKM_DES_KEY_GEN\n");
			loginUser(hSession);
			hKey = 0;
			if((generateKey(&keyMechanism, &hKey, &hSession) == 0))//gen key
			{
				//DigestInit
				printf("  call C_DigestInit on slot %lu ...", slotID);
				rv = pFunctionList->C_DigestInit(hSession, mechanism);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");
				
				//C_DigestKey
				printf("  call C_DigestKey on slot %lu ...", slotID);
				rv = pFunctionList->C_DigestKey(hSession, hKey);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");
				
				//C_DigestFinal
				printf("  call C_DigestFinal on slot %lu with NULL_PTR ...", slotID);
				rv = pFunctionList->C_DigestFinal(hSession, NULL_PTR, dataDigestLen);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");
				
				free(dataDigest);
				dataDigest = malloc((*dataDigestLen) * sizeof(CK_BYTE));
				
				//C_DigestFinal
				printf("  call C_DigestFinal on slot %lu ...", slotID);
				rv = pFunctionList->C_DigestFinal(hSession, dataDigest, dataDigestLen);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");
				free(dataDigest);
			}
			logoutUser(hSession);
		}
		else printf(" skipping C_DigestKey because it's not supported\n");
		
	}
	else
	{
		if(k != -1)	printf(" skip Digesting(%s) because mecha is not yet implemented ...\n", MECHANISM_LIST[k].eleName);
		else printf(" skip Digesting(mecha = %lu) because mecha is not yet implemented ...\n", mechanism->mechanism);
	}

	
	return 0;
}

int
testCaseDigestSimple()
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
		
	printf("Testing case: DigestSimple ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseDigestSimpleDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION | CKF_RW_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION | CKF_RW_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		for(j=0;j<pMechaInfoListLen;j++)//each mechanism
		{
			if(!(pMechaInfoList[j].info.flags & CKF_DIGEST)) continue; //that is capable of digesting
			mechanism.mechanism = pMechaInfoList[j].type;
			
			digestSimple(slotID, &mechanism, hSession, data, dataDigest, &dataDigestLen);
		}	
		
		closeSession(hSession);	
	}
	
	//finalize
	printf(" call Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	free(pTokenList);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
