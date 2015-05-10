const char *testCaseSingleKeyGenerationDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_GenerateKey", "C_DestroyObject", "C_OpenSession", "C_CloseSession", "C_Login", "C_Logout",NULL};


int
testCaseSingleKeyGeneration()
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
	CK_ATTRIBUTE_PTR template = NULL_PTR;
	CK_ULONG templateSize = 0;
	CK_OBJECT_HANDLE hKey;
	int i,j,k;
		
	printf("Testing case: SingleKeyGeneration ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSingleKeyGenerationDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		for(j=0;j<pMechaInfoListLen;j++)//each mechanism
		{
			if(!(pMechaInfoList[j].info.flags & CKF_GENERATE)) continue; //that is capable of generating
			mechanism.mechanism = pMechaInfoList[j].type;
			
			k = searchEleName(mechanism.mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
			
			rv = setParamsForKey(&mechanism, &template, &templateSize);
			if(rv == 0)
			{
				//GenerateKey
				if(k != -1)	printf(" call C_GenerateKey(%s) on slot %lu ...", MECHANISM_LIST[k].eleName, slotID);
				else printf(" call C_GenerateKey(mecha = %lu) on slot %lu ...", mechanism.mechanism, slotID);
				rv = pFunctionList->C_GenerateKey(hSession, &mechanism, template, templateSize, &hKey);
				if(rv != CKR_OK)
				{
					returnValuePrinting("C_GenerateKey", rv);
				}
				else
				{
					 printf(" success\n");
					if(checkFunctionImplemented("C_DestroyObject")!=-1)	pFunctionList->C_DestroyObject(hSession, hKey);
				}
			}
			else
			{
				if(k != -1)	printf(" skip C_GenerateKey(%s) because mecha is not yet implemented ...\n", MECHANISM_LIST[k].eleName);
				else printf(" skip C_GenerateKey(mecha = %lu) because mecha is not yet implemented ...\n", mechanism.mechanism);
			}
			
			
		}		
				
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
