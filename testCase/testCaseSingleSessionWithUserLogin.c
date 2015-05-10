const char *testCaseSingleSessionWithUserLoginDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_Login", "C_Logout", "C_GetSessionInfo", "C_CloseSession", NULL};


int
testCaseSingleSessionWithUserLogin()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_SESSION_INFO info;
	int i;
		
	printf("Testing case: SingleSessionHandling ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSingleSessionWithUserLoginDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		/* --------- CKF_RW_SESSION ---------*/
		//OpenSession with CKF_RW_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_RW_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_RW_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_SESSION_PARALLEL_NOT_SUPPORTED) printf(" Unexpected return value %lu without CKF_SERIAL_SESSION flag. Expected CKR_SESSION_PARALLEL_NOT_SUPPORTED.\n", rv);
		else printf(" failed as expected\n");
		
		//GetSessionInfo
		printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv != CKR_SESSION_HANDLE_INVALID){printf(" Unexpected return value %lu. Expected CKR_SESSION_HANDLE_INVALID.\n", rv);}
		else printf(" failed as expected\n");
		
		//CloseSession
		printf(" call C_CloseSession on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_CloseSession(hSession);
		if(rv != CKR_SESSION_HANDLE_INVALID){printf(" Unexpected return value %lu. Expected CKR_SESSION_HANDLE_INVALID.\n", rv);}
		else printf(" failed as expected\n");
		
		
		/* -------- CKF_RW_SESSION & CKF_SERIAL_SESSION ---------*/
		//OpenSession with CKF_RW_SESSION & CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_RW_SESSION & CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_RW_SESSION | CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//GetSessionInfo
		printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else if(info.state == CKS_RW_PUBLIC_SESSION) printf(" success\n");
		else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_PUBLIC_SESSION);}
		
		if(sArguments.uPin)
		{
			//Login
			printf(" call C_Login on slot %lu as User ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK)
			{
				returnTestCase(rv, 1);
				exit(EXIT_FAILURE);
			}
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_USER_FUNCTIONS) printf(" success(could verify state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_USER_FUNCTIONS);}
			
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_PUBLIC_SESSION) printf(" success(could verify state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_PUBLIC_SESSION);}	
		}
		
		if(sArguments.soPin)
		{
			//Login
			printf(" call C_Login on slot %lu as SO ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_SO, (CK_UTF8CHAR_PTR) sArguments.soPin, strlen(sArguments.soPin));
			if(rv != CKR_OK) 
			{
				returnTestCase(rv, 1);
				exit(EXIT_FAILURE);
			}
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_SO_FUNCTIONS) printf(" success(could verify state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_SO_FUNCTIONS);}
			
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_PUBLIC_SESSION) printf(" success(could verify state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_PUBLIC_SESSION);}	
		}

		//CloseSession
		printf(" call C_CloseSession on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_CloseSession(hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");	
		
		//GetSessionInfo after close
		printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv == CKR_SESSION_HANDLE_INVALID) printf(" success(session handle is invalid after close)\n");
		else returnTestCase(rv, 0);
		
		/* ------ CKF_SERIAL_SESSION ---------*/
		//OpenSession with CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//GetSessionInfo
		printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else if(info.state == CKS_RO_PUBLIC_SESSION) printf(" success\n");
		else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_PUBLIC_SESSION);}
		
		if(sArguments.uPin)
		{
			//Login
			printf(" call C_Login on slot %lu as User ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK)
			{
				returnTestCase(rv, 1);
				exit(EXIT_FAILURE);
			}
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RO_USER_FUNCTIONS) printf(" success(could verify state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_USER_FUNCTIONS);}
			
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RO_PUBLIC_SESSION) printf(" success(could verify state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_PUBLIC_SESSION);}
		}
		
		if(sArguments.soPin)
		{
			//Login
			printf(" call C_Login on slot %lu as SO ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.soPin, strlen(sArguments.soPin));
			if(rv == CKR_ARGUMENTS_BAD)
			{
				printf(" SO do not have a R/O session\n");
			}
			else printf(" unexpected return value %lu\n", rv);	
		}

		//CloseSession
		printf(" call C_CloseSession on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_CloseSession(hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");	
		
		//GetSessionInfo after close
		printf(" call C_GetSessionInfo on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv == CKR_SESSION_HANDLE_INVALID) printf(" success(session handle is invalid after close)\n");
		else returnTestCase(rv, 0);
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
