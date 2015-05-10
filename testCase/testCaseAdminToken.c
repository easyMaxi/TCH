const char *testCaseAdminTokenDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_GetTokenInfo", "C_Login", "C_Logout", "C_InitToken", "C_InitPIN", "C_CloseSession", NULL};


int
testCaseAdminToken()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_TOKEN_INFO tokenInfo;
	CK_UTF8CHAR label[32];
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	memset(label, ' ', sizeof(label));
	memcpy(label, "TCH Testtoken", strlen("TCH Testtoken"));
	int i;
		
	printf("Testing case: AdminToken ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseAdminTokenDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	if(sArguments.initiate != 1)
	{
		printf(" expected argument '-i' was not ommited\n");
		return 0;
	}
	if(!sArguments.soPin)
	{
		printf(" expected argument '-s PIN' was not ommited\n");
		return 0;
	}
	if(!sArguments.uPin)
	{
		printf(" expected argument '-u PIN' was not ommited\n");
		return 0;
	}
	printf(" Dependencies are fine\n");
	
	//initialize
	printf(" call C_Initialize ...");
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	//GetSlotList -> Tokenlist
	findActiveTokens(&pTokenList, &ulTokenCount);
	
	for(i=0;i<ulTokenCount;i++)
	{
		slotID = pTokenList[i];
		
		getTokenInfo(slotID, &tokenInfo);
		if(tokenInfo.flags & CKF_PROTECTED_AUTHENTICATION_PATH)
		{
			printf(" flag CKF_PROTECTED_AUTHENTICATION_PATH is set. Currently not supported\n");
			return 0;
		}
		if(tokenInfo.flags & CKF_TOKEN_INITIALIZED)
		{
			printf(" flag CKF_TOKEN_INITIALIZED is set. So the token will be reinitialized\n");
			return 0;
		}
		
		
		
		if(sArguments.soPin)
		{

			//InitToken
			printf(" call C_InitToken on slot %lu as SO ...", slotID);
			rv = pFunctionList->C_InitToken(slotID, (CK_UTF8CHAR_PTR) sArguments.soPin, strlen(sArguments.soPin), label);
			if(rv != CKR_OK)
			{
				returnTestCase(rv, 1);
				return 0;
			}
			else printf(" success. SO PIN is %s\n", sArguments.soPin);

			//openSession
			printf(" call C_OpenSession on slot %lu with CKF_RW_SESSION & CKF_SERIAL_SESSION...", slotID);
			rv = pFunctionList->C_OpenSession(slotID, CKF_RW_SESSION | CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");

			//login
			rv = pFunctionList->C_Login(hSession, CKU_SO, (CK_UTF8CHAR_PTR) sArguments.soPin, strlen(sArguments.soPin));
			if(rv != CKR_OK) 
			{
				returnTestCase(rv, 1);
				exit(EXIT_FAILURE);
			}
			else printf(" success\n");

			//InitPIN
			printf(" call C_InitPIN on slot %lu as SO ...", slotID);
			rv = pFunctionList->C_InitPIN(hSession, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK)
			{
				returnTestCase(rv, 1);
				return 0;
			}
			else printf(" success. User PIN is %s\n", sArguments.soPin);
		
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");	
			
			//CloseSession
			printf(" call C_CloseSession on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_CloseSession(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");
		}
		
	}
	
	//finalize
	printf(" call C_Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	

	free(pTokenList);
	printf(" ... case successful\n");
	
	return CKR_OK;
}
