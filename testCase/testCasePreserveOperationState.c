const char *testCasePreserveOperationStateDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_CloseSession", "C_GetOperationState", "C_SetOperationState", NULL};

int
testCasePreserveOperationState(){
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_BYTE_PTR pOperationState = NULL_PTR;
	CK_ULONG ulOperationStateLen = 0;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	int i;
	
	printf("Testing case: PreserveOperationState ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCasePreserveOperationStateDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
	
		printf(" call C_GetOperationState NULL_PTR...");
		rv = pFunctionList->C_GetOperationState(hSession, NULL_PTR, &ulOperationStateLen);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		pOperationState = malloc(ulOperationStateLen * sizeof(CK_BYTE));
		
		printf(" call C_GetOperationState ...");
		rv = pFunctionList->C_GetOperationState(hSession, pOperationState, &ulOperationStateLen);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
	
		printf(" call C_SetOperationState ...");
		rv = pFunctionList->C_SetOperationState(hSession, pOperationState, ulOperationStateLen, 0, 0);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
	
		closeSession(hSession);
	
		
	}
	finalizeToken();
	
	printf(" ... case successful\n");
	
	return CKR_OK;
}
