const char *testCaseObjectSearchDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_FindObjects", "C_FindObjectsFinal", "C_FindObjectsInit", "C_OpenSession", "C_CloseSession", "C_Login", "C_Logout",NULL};

int
testCaseObjectSearch()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_OBJECT_HANDLE hObject;
	int i;
	
	//declarations for the objects
	CK_BBOOL true = CK_TRUE;
	CK_UTF8CHAR application[] = {"TCH"};
	CK_BYTE dataValue[] = {"Data value"};
	CK_OBJECT_CLASS dataClass = CKO_DATA;
	CK_ULONG dataTemplateSize = 4;
	CK_ATTRIBUTE dataTemplate[] = 
	{   
		{CKA_CLASS, &dataClass, sizeof(dataClass)},
		{CKA_TOKEN, &true, sizeof(true)},
		{CKA_APPLICATION, application, sizeof(application)-1},
		{CKA_VALUE, dataValue, sizeof(dataValue)} 
	};
		
	printf("Testing case: ObjectSearch ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseObjectSearchDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		if(sArguments.uPin)
		{
			//Login
			printf(" call C_Login on slot %lu as User ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK) exit(EXIT_FAILURE);
			else printf(" success\n");	
		}
		
		int count = 0;
		if((rv = generateObject(slotID, hSession, &hObject, dataTemplate, dataTemplateSize)) == 0) count++;
		if((rv = generateObject(slotID, hSession, &hObject, dataTemplate, dataTemplateSize)) == 0) count++;
		if((rv = generateObject(slotID, hSession, &hObject, dataTemplate, dataTemplateSize)) == 0) count++;

		printf(" could create %d data objects. So we should find %d more than at the start\n", count, count);
		
		findObjects(slotID, hSession, dataTemplate);
		
				
		if(sArguments.uPin)
		{
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");	
		}
		//CloseSession
		printf(" call C_CloseSession on slot %lu...", slotID);
		rv = pFunctionList->C_CloseSession(hSession);
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
