const char *testCaseSlotEventsDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_WaitForSlotEvent", NULL};


int
testCaseSlotEvents()
{
	CK_RV rv;
	CK_SLOT_INFO info;
	CK_TOKEN_INFO tInfo;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	int i;
		
	printf("Testing case: SlotEvents ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSlotEventsDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		//GetSlotInfo
		printf(" call GetSlotInfo on slot %lu ...", slotID);
		rv = pFunctionList->C_GetSlotInfo(slotID, &info);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		if((info.flags & (CKF_TOKEN_PRESENT | CKF_REMOVABLE_DEVICE)))
		{
			printf(" A removable token was detected.\n");
		}
		else
		{
			printf(" No removable token was detected on slot %lu.\n", slotID);
			continue;
		}
		
		//remove the token
		printf(" call C_WaitForSlotEvent on slot %lu ...\n", slotID);
		printf("   Please remove the token on slot %lu.\n", slotID);
		rv = pFunctionList->C_WaitForSlotEvent(0, &slotID, NULL_PTR);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		printf(" call C_GetTokenInfo on slot %lu ...", slotID);
		rv = pFunctionList->C_GetTokenInfo(slotID, &tInfo);
		if (rv != CKR_TOKEN_NOT_PRESENT)
		{ 
			printf(" We had not the expected return value: CKR_TOKEN_NOT_PRESENT. Instead Nr. %lu\n", rv);
		}
		else
		{
			printf(" The token was removed.\n");
		}
		
		//insert the token
		printf(" call C_WaitForSlotEvent on slot %lu ...\n", slotID);
		printf("   Please insert the token on slot %lu.\n", slotID);
		rv = pFunctionList->C_WaitForSlotEvent(0, &slotID, NULL_PTR);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");

		printf(" call C_GetSlotInfo on slot %lu ...", slotID);
		rv = pFunctionList->C_GetSlotInfo(slotID, &info);
		if (rv != CKR_OK) printf(" we had not the expected return value: CKR_OK. Instead Nr. %lu\n", rv);
		if(info.flags & CKF_TOKEN_PRESENT)
		{
			printf(" the token was detected.\n");
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
