const char *testCaseSlotInformationDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_GetSlotInfo", NULL};


int
testCaseSlotInformation()
{
	CK_RV rv;
	CK_SLOT_INFO info;
	CK_ULONG ulSlotCount;
	CK_SLOT_ID_PTR pSlotList;
	CK_SLOT_ID slotID;
	int i;
		
	printf("Testing case: SlotInformation ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSlotInformationDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	//initialize
	printf(" call Initialize ...");
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	//GetSlotList -> Tokenlist
	printf(" call GetSlotList with CK_FALSE and NULL_PTR ...");
	rv = pFunctionList->C_GetSlotList(CK_FALSE, NULL_PTR, &ulSlotCount); //we need the number of slots
	if(rv != CKR_OK) returnTestCase(rv, 0);	
	else printf(" success\n");
	
	pSlotList = malloc(ulSlotCount * sizeof(CK_SLOT_ID)); //make a list
	
	printf(" call GetSlotList with CK_FALSE ...");
	rv = pFunctionList->C_GetSlotList(CK_FALSE, pSlotList, &ulSlotCount); //and fill the list
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	for(i=0;i<ulSlotCount;i++)
	{
		slotID = pSlotList[i];
		
		//GetSlotInfo
		printf(" call GetSlotInfo on slot %lu ...", slotID);
		rv = pFunctionList->C_GetSlotInfo(slotID, &info);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
	}
	
	//finalize
	printf(" call Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	free(pSlotList);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
