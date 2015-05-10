const char *testCaseMultipleSessionWithUserLoginDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_Login", "C_Logout", "C_GetSessionInfo", "C_CloseAllSessions", NULL};

int
testCaseMultipleSessionWithUserLogin()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession, hSession2;
	CK_SESSION_INFO info;
	int i;
		
	printf("Testing case: MultipleSessionHandling ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseMultipleSessionWithUserLoginDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	//initialize
	printf(" call C_Initialize ...");
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
		
		/* -------- CKF_RW_SESSION & CKF_SERIAL_SESSION ---------*/
		//OpenSession with CKF_RW_SESSION & CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_RW_SESSION & CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_RW_SESSION | CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		//OpenSession with CKF_RW_SESSION & CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_RW_SESSION & CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_RW_SESSION | CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession2);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
		
		if(sArguments.uPin)
		{
			//Login
			printf(" call C_Login on slot %lu as User ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK) exit(EXIT_FAILURE);
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 1...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_USER_FUNCTIONS) printf(" success(could verify session state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_USER_FUNCTIONS);}
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 2...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession2, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_USER_FUNCTIONS) printf(" success(could verify session state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_USER_FUNCTIONS);}
			
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 1...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_PUBLIC_SESSION) printf(" success(could verify session state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_PUBLIC_SESSION);}	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 2...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession2, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RW_PUBLIC_SESSION) printf(" success(could verify session state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RW_PUBLIC_SESSION);}	
		}

		//CloseSession
		printf(" call C_CloseAllSessions on slot %lu ...", slotID);
		rv = pFunctionList->C_CloseAllSessions(slotID);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");	
		
		//GetSessionInfo after close
		printf(" call C_GetSessionInfo on slot %lu with session handle 1...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv == CKR_SESSION_HANDLE_INVALID) printf(" success(session handle is invalid after close)\n");
		else returnTestCase(rv, 0);
		
		//GetSessionInfo
		printf(" call C_GetSessionInfo on slot %lu with session handle 2...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession2, &info);
		if(rv == CKR_SESSION_HANDLE_INVALID) printf(" success(session handle is invalid after close)\n");
		else returnTestCase(rv, 0);
		
		/* ------ CKF_SERIAL_SESSION ---------*/
		hSession = 0;
		hSession2 = 0;
		//OpenSession with CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");

		//OpenSession with CKF_SERIAL_SESSION
		printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION...", slotID);
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession2);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");
			
		if(sArguments.uPin)
		{
			//Login
			printf(" call C_Login on slot %lu as User ...", slotID);
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK) exit(EXIT_FAILURE);
			else printf(" success\n");	
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 1...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RO_USER_FUNCTIONS) printf(" success(could verify session state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_USER_FUNCTIONS);}
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 2...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession2, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RO_USER_FUNCTIONS) printf(" success(could verify session state after login)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_USER_FUNCTIONS);}
			
			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 1...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RO_PUBLIC_SESSION) printf(" success(could verify session state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_PUBLIC_SESSION);}
			
			//GetSessionInfo
			printf(" call C_GetSessionInfo on slot %lu with session handle 2...", slotID);
			rv = pFunctionList->C_GetSessionInfo(hSession2, &info);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else if(info.state == CKS_RO_PUBLIC_SESSION) printf(" success(could verify session state after logout)\n");
			else{printf(" success in calling but session state ist wrong: %lu, expected %d\n", info.state, CKS_RO_PUBLIC_SESSION);}
		}

		//CloseSession
		printf(" call C_CloseAllSessions on slot %lu ...", slotID);
		rv = pFunctionList->C_CloseAllSessions(slotID);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");	
		
		
		//GetSessionInfo after close
		printf(" call C_GetSessionInfo on slot %lu with session handle 1...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession, &info);
		if(rv == CKR_SESSION_HANDLE_INVALID) printf(" success(session handle is invalid after close)\n");
		else returnTestCase(rv, 0);
		
		//GetSessionInfo
		printf(" call C_GetSessionInfo on slot %lu with session handle 2...", slotID);
		rv = pFunctionList->C_GetSessionInfo(hSession2, &info);
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
