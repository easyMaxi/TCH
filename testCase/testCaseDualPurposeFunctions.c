const char *testCaseDualPurposeFunctionsDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_CloseSession", "C_DigestEncryptUpdate", "C_DecryptDigestUpdate", "C_SignEncryptUpdate", "C_DecryptVerifyUpdate", NULL};

int
testCaseDualPurposeFunctions(){
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	int i;
	
	printf("Testing case: DualPurposeFunctions ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseDualPurposeFunctionsDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");

	initToken();
	
	//GetSlotList -> Tokenlist
	findActiveTokens(&pTokenList, &ulTokenCount);
	
	for(i=0;i<ulTokenCount;i++)
	{
		slotID = pTokenList[i];


		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSIONN...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
	
		printf("   testCase is not yet completly implemented.");
	
		closeSession(hSession);
	
		
	}
	finalizeToken();
	
	printf(" ... case successful\n");
	
	return CKR_OK;
}
