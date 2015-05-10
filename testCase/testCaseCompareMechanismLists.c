const char *testCaseCompareMechanismListsDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_GetMechanismInfo", "C_GetMechanismList", NULL};
int checkForMechanismInList(CK_MECHANISM_TYPE_PTR mechaList, CK_ULONG listSize, CK_ULONG mechaNum);

int
testCaseCompareMechanismLists()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_ULONG mechaCount;
	CK_MECHANISM_INFO info;
	CK_MECHANISM_TYPE_PTR pMechaList = NULL_PTR;
	int i;
		
	printf("Testing case: CompareMechanismLists ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseCompareMechanismListsDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	
	//initialize
	printf(" call Initialize ...");
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	//GetSlotList -> Tokenlist
	printf(" call C_GetSlotList with CK_TRUE and NULL_PTR ...");
	rv = pFunctionList->C_GetSlotList(CK_TRUE, NULL_PTR, &ulTokenCount); //we need the number of tokens
	if(rv != CKR_OK) returnTestCase(rv, 0);	
	else printf(" success\n");
	
	pTokenList = malloc(ulTokenCount * sizeof(CK_SLOT_ID)); //make a list
	
	printf(" call C_GetSlotList with CK_TRUE ...");
	rv = pFunctionList->C_GetSlotList(CK_TRUE, pTokenList, &ulTokenCount); //and fill the list
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	for(i=0;i<ulTokenCount;i++)
	{
		slotID = pTokenList[i];
		
		//GetMechanismList
		printf(" call GetMechanismList with NULL_PTR on slot %lu ...", slotID);
		rv = pFunctionList->C_GetMechanismList(slotID, NULL_PTR, &mechaCount);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		pMechaList = malloc( mechaCount * sizeof(CK_MECHANISM_TYPE) );
		
		printf(" call GetMechanismList on slot %lu ...", slotID);
		rv = pFunctionList->C_GetMechanismList(slotID, pMechaList, &mechaCount);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//phase 1 (call getInfo over all known mechanisms)
		int j = 0;
		int errorCount = 0;
		printf(" iterate over all mechanisms and call C_GetMechanismInfo on slot %lu ...", slotID);
		while((MECHANISM_LIST[j].eleValue!=0) & (MECHANISM_LIST[j].eleName != NULL))//each known mechanism
		{
			rv = pFunctionList->C_GetMechanismInfo(slotID, MECHANISM_LIST[j].eleValue, &info);
			if(rv != CKR_MECHANISM_INVALID)	
			{
				if(checkForMechanismInList(pMechaList, mechaCount, MECHANISM_LIST[j].eleValue) == 1)
				{
					//found in list
				}
				else
				{
					errorCount++;
				}
			}
			j++;
		}
		printf(" compared with MechanismList and the result is off by %d mechanisms\n", errorCount);
		
		//phase 2 (call getInfo over MechanismList)
		printf(" iterate over mechanismlist(C_GetMechanismList) and call C_GetMechanismInfo\n");
		for(j=0;j<mechaCount;j++)//each mechanism in list
		{
			rv = pFunctionList->C_GetMechanismInfo(slotID, pMechaList[j], &info);
			if(rv == CKR_MECHANISM_INVALID)	
			{
				printf("  unexpected unsupported mecha: %lu\n", pMechaList[j]);
			}
			j++;
		}
	}
	
	//finalize
	printf(" call Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	

	free(pTokenList);
	free(pMechaList);
	printf(" ... case successful\n");
	
	return CKR_OK;
}


int
checkForMechanismInList(CK_MECHANISM_TYPE_PTR mechaList, CK_ULONG listSize, CK_ULONG mechaNum)
{
	int i;
	for(i=0;i<listSize;i++)
	{
		if(mechaList[i] == mechaNum) return 1;
	}
	
	return 0;
}
