const char *testCaseObjectHandlingDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_OpenSession", "C_CloseSession", "C_Login", "C_Logout", "C_CreateObject", "_DestroyObject", NULL};

int
testCaseObjectHandling()
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
	CK_BYTE dataValue[] = {"Data example"};
	CK_OBJECT_CLASS dataClass = CKO_DATA;
	CK_ULONG dataTemplateSize = 4;
	CK_ATTRIBUTE dataTemplate[] = 
	{   
		{CKA_CLASS, &dataClass, sizeof(dataClass)},
		{CKA_TOKEN, &true, sizeof(true)},
		{CKA_APPLICATION, application, sizeof(application)-1},
		{CKA_VALUE, dataValue, sizeof(dataValue)} 
	};
	//declaration for GetAttributeValue
	CK_BYTE_PTR pApplication, pValue;
	CK_ULONG dataTemplateGetValueSize = 2;
	CK_ATTRIBUTE dataTemplateGetValue[] = 
	{   
		{CKA_APPLICATION, NULL_PTR, 0},
		{CKA_VALUE, NULL_PTR, 0}
	};
	//declaration for C_CopyObject
	CK_OBJECT_HANDLE hObjectCopy;
	//declaration for C_GetObjectSize
	CK_ULONG uSize;
	//declaration for C_SetAttributeValue
	CK_UTF8CHAR label[] = {"New TCH Label"};
	CK_ATTRIBUTE setAttributeTemplate = { CKA_LABEL, label, sizeof(label)-1 };

		
	printf("Testing case: ObjectHandling ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseObjectHandlingDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		if((generateObject(slotID, hSession, &hObject, dataTemplate, dataTemplateSize)) == 0)//C_GenerateObject
		{
		
			if(checkFunctionImplemented("C_SetAttributeValue"))
			{
				printf(" call C_SetAttributeValue on slot %lu ...", slotID);
				rv = pFunctionList->C_SetAttributeValue(hSession, hObject, &setAttributeTemplate, 1);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");	
			}
			else printf(" skipping C_SetAttributeValue because it's not supported\n");

			if(checkFunctionImplemented("C_GetAttributeValue"))//C_GetAttributeValue
			{	
				printf(" call C_GetAttributeValue on slot %lu ...", slotID);
				rv = pFunctionList->C_GetAttributeValue(hSession, hObject, (CK_ATTRIBUTE_PTR) &dataTemplateGetValue, dataTemplateGetValueSize);
				if(rv == CKR_OK)
				{		
					printf(" success\n");	
					//C_GetAttribute Vallue
					pApplication = (CK_BYTE_PTR) malloc(dataTemplateGetValue[0].ulValueLen);
					pValue = (CK_BYTE_PTR) malloc(dataTemplateGetValue[1].ulValueLen);
					
					dataTemplateGetValue[0].pValue = pApplication;
					dataTemplateGetValue[1].pValue = pValue;
					
					rv = pFunctionList->C_GetAttributeValue(hSession, hObject, (CK_ATTRIBUTE_PTR) &dataTemplateGetValue, dataTemplateGetValueSize);
					if(rv == CKR_OK)
					{						
						printf(" got attribute values Label: %.*s, Value: %.*s\n",
							(int) dataTemplateGetValue[0].ulValueLen,
							(char *) dataTemplateGetValue[0].pValue,
							(int) dataTemplateGetValue[1].ulValueLen, 
							(char *) dataTemplateGetValue[1].pValue
						);
					}
					else returnTestCase(rv, 0);
					
					free(pApplication);
					free(pValue);
				}
				else returnTestCase(rv, 0);	
			}
			else printf(" skipping C_GetAttributeValue because it's not supported\n");
			
			if(checkFunctionImplemented("C_GetObjectSize"))
			{
				printf(" call C_GetObjectSize on slot %lu ...", slotID);
				rv = pFunctionList->C_GetObjectSize(hSession, hObject, &uSize);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success (size: %lu)\n", uSize);	
			}
			else printf(" skipping C_GetObjectSize because it's not supported\n");
			
			if(checkFunctionImplemented("C_CopyObject"))
			{
				printf(" call C_CopyObject on slot %lu ...", slotID);
				rv = pFunctionList->C_CopyObject(hSession, hObject, dataTemplate, dataTemplateSize, &hObjectCopy);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success (copy object handle: %lu, old object handle: %lu)\n", hObject, hObjectCopy);	

				//DestroyObject
				printf(" call C_DestroyObject on slot %lu on copied object ...", slotID);
				rv = pFunctionList->C_DestroyObject(hSession, hObjectCopy);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");	

			}
			else printf(" skipping C_CopyObject because it's not supported\n");
			
			//DestroyObject
			printf(" call C_DestroyObject on slot %lu ...", slotID);
			rv = pFunctionList->C_DestroyObject(hSession, hObject);
			if(rv != CKR_OK) returnTestCase(rv, 0);
			else printf(" success\n");	

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
