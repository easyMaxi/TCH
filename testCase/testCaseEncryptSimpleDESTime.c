const char *testCaseEncryptSimpleDESTimeDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_EncryptInit", "C_Encrypt", "C_GenerateKey", NULL};


int
encryptDES(CK_BYTE_PTR data, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession)
{
	CK_RV rv;
	
	CK_BYTE_PTR dataEnc = NULL_PTR;
	CK_ULONG dataEncLen = 0;
	
	CK_MECHANISM encryptMechanism = {
		CKM_DES_ECB, NULL_PTR, 0
	};
	
	//C_EncryptInit
	rv = pFunctionList->C_EncryptInit(*hSession, &encryptMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnTestCase(rv, 1);
		return 0;
	}
	
	//C_Encrypt NULL_PTR
	rv = pFunctionList->C_Encrypt(*hSession, data, strlen((char *)data), NULL_PTR, &dataEncLen);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	
	dataEnc = malloc(dataEncLen * sizeof(CK_BYTE));
	
	//C_Encrypt
	rv = pFunctionList->C_Encrypt(*hSession, data, strlen((char *)data), dataEnc, &dataEncLen);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	
	free(dataEnc);
	
	return 0;
}

int
testCaseEncryptSimpleDESTime()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_BYTE_PTR data;
	CK_MECHANISM keyMechanism = {
		CKM_DES_KEY_GEN, NULL_PTR, 0
	};
	CK_OBJECT_HANDLE hKey = 0;
	clock_t start;

	
	int i;
		
	printf("Testing case: EncryptSimpleDESTime ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseEncryptSimpleDESTimeDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		if(sArguments.uPin)
		{
			//Login
			printf(" call C_Login on slot %lu as User ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK) exit(EXIT_FAILURE);
			else printf(" success\n");	
		}
		
		generateKey(&keyMechanism, &hKey, &hSession);
		
		
			
		printf(" encrypt ...\n");
		
		generateSampleData(16, &data);
		start = time(NULL);
		encryptDES(data, &hKey, &hSession);
		printf("  Could encrypt %zu CK_BYTEs in %f sec.\n", strlen((char *) data), (double)(time(NULL) - start));
		
		generateSampleData(1048576, &data);
		start = time(NULL);
		encryptDES(data, &hKey, &hSession);
		printf("  Could encrypt %zu CK_BYTEs in %f sec.\n", strlen((char *) data), (double)(time(NULL) - start));
		
		generateSampleData(33554432, &data);
		start = time(NULL);
		encryptDES(data, &hKey, &hSession);
		printf("  Could encrypt %zu CK_BYTEs in %f sec.\n", strlen((char *) data), (double)(time(NULL) - start));



		if(sArguments.uPin)
		{
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");		
		}
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
