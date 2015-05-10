const char *testCaseChangePinDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_Login", "C_Logout", "C_SetPIN", "C_CloseSession", NULL};


int
testCaseChangePin()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_UTF8CHAR newPin[] = {"0000"};
	int sizeOfNewPin = 4;
	int i;
		
	printf("Testing case: ChangePin ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseChangePinDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	if(sArguments.mode != 1)
	{
		printf(" expected argument '-o' was not ommited\n");
		return 0;
	}
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
			
			//SetPIN
			printf(" call C_SetPIN on slot %lu as User ...", slotID);
			rv = pFunctionList->C_SetPIN(hSession, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin), newPin, sizeOfNewPin);
			if(rv != CKR_OK)
			{
				returnValuePrinting("C_SetPIN", rv);
				returnTestCase(rv, 0);
			}
			else{
				printf(" success. New PIN is %s\n", newPin);
				
				//Logout
				printf(" call C_Logout on slot %lu ...", slotID);
				rv = pFunctionList->C_Logout(hSession);
				if(rv != CKR_OK)
				{
					returnValuePrinting("C_Logout", rv);
					returnTestCase(rv, 0);
				}
				else printf(" success\n");	
				
			
				//Login
				printf(" call C_Login on slot %lu as User with new PIN ...", slotID);
				rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) newPin, sizeOfNewPin);
				if(rv != CKR_OK)
				{
					returnTestCase(rv, 1);
					exit(EXIT_FAILURE);
				}
				else printf(" success\n");	
				
				//SetPIN
				printf(" call C_SetPIN on slot %lu as User set PIN back to %s...", slotID, sArguments.uPin);
				rv = pFunctionList->C_SetPIN(hSession, newPin, sizeOfNewPin, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
				if(rv != CKR_OK)
				{
					returnValuePrinting("C_SetPIN", rv);
					returnTestCase(rv, 0);
				}
				else printf(" success. New PIN is %s\n", sArguments.uPin);
			}
			

			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK)
			{
				returnValuePrinting("C_Logout", rv);
				returnTestCase(rv, 0);
			}
			else printf(" success\n");
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
			
			//SetPIN
			printf(" call C_SetPIN on slot %lu as SO ...", slotID);
			rv = pFunctionList->C_SetPIN(hSession, (CK_UTF8CHAR_PTR) sArguments.soPin, strlen(sArguments.soPin), newPin, sizeOfNewPin);
			if(rv != CKR_OK)
			{
				returnValuePrinting("C_SetPIN", rv);
				returnTestCase(rv, 0);
			}
			else
			{
				printf(" success. New PIN is %s\n", newPin);
				
				//Logout
				printf(" call C_Logout on slot %lu ...", slotID);
				rv = pFunctionList->C_Logout(hSession);
				if(rv != CKR_OK)
				{
					returnValuePrinting("C_Logout", rv);
					returnTestCase(rv, 0);
				}
				else printf(" success\n");	
				
				//Login
				printf(" call C_Login on slot %lu as SP with new PIN ...", slotID);
				rv = pFunctionList->C_Login(hSession, CKU_SO, (CK_UTF8CHAR_PTR) newPin, sizeOfNewPin);
				if(rv != CKR_OK)
				{
					returnTestCase(rv, 1);
					exit(EXIT_FAILURE);
				}
				else printf(" success\n");	
				
				//SetPIN
				printf(" call C_SetPIN on slot %lu as SO set PIN back to %s...", slotID, sArguments.uPin);
				rv = pFunctionList->C_SetPIN(hSession, newPin, sizeOfNewPin, (CK_UTF8CHAR_PTR) sArguments.soPin, strlen(sArguments.soPin));
				if(rv != CKR_OK)
				{
					returnValuePrinting("C_SetPIN", rv);
					returnTestCase(rv, 0);
				}
				else printf(" success. New PIN is %s\n", sArguments.soPin);
			
			}
			

			//Logout
			printf(" call C_Logout on slot %lu ...", slotID);
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK)
			{
				returnValuePrinting("C_Logout", rv);
				returnTestCase(rv, 0);
			}
			else printf(" success\n");
		}

		//CloseSession
		printf(" call C_CloseSession on slot %lu with the previous session handle...", slotID);
		rv = pFunctionList->C_CloseSession(hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");	
		
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
