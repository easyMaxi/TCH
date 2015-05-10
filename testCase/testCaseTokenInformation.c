const char *testCaseTokenInformationDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_GetTokenInfo", "C_GetMechanismList", NULL};

int
testCaseTokenInformation()
{
	CK_RV rv;
	CK_TOKEN_INFO info;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_ULONG mechaCount;
	CK_MECHANISM_TYPE_PTR pMechaList = NULL_PTR;
	int i;
		
	printf("Testing case: TokenInformation ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseTokenInformationDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		//GetTokenInfo
		printf(" call C_GetTokenInfo on slot %lu ...", slotID);
		rv = pFunctionList->C_GetTokenInfo(slotID, &info);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//GetMechanismList
		printf(" call GetMechanismList with NULL_PTR on slot %lu ...", slotID);
		rv = pFunctionList->C_GetMechanismList(slotID, NULL_PTR, &mechaCount);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		printf(" call GetMechanismList on slot %lu ...", slotID);
		rv = pFunctionList->C_GetMechanismList(slotID, pMechaList, &mechaCount);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
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
