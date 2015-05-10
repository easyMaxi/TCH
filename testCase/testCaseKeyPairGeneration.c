const char *testCaseKeyPairGenerationDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_GenerateKeyPair", NULL};

int
testCaseKeyPairGeneration()
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
	CK_OBJECT_HANDLE hPublicKey, hPrivateKey;
	CK_ULONG publicKeyAttributeCount, privateKeyAttributeCount;
	CK_ATTRIBUTE_PTR publicKeyTemplate, privateKeyTemplate;
	int i,j,k;
	
	/* should be replaced by a working copy of  generateKeyPair*/
	CK_ULONG modulusBits = 768;
	CK_BYTE publicExponent[] = { 3 };
	CK_BYTE subject[] = {1};
	CK_BYTE id[] = {123};
	CK_BBOOL true = CK_TRUE;
	CK_ULONG publicKeyTemplateAttributeCountRSA = 5;
	CK_ATTRIBUTE publicKeyTemplateRSA[] = {
		{CKA_ENCRYPT, &true, sizeof(true)},
		{CKA_VERIFY, &true, sizeof(true)},
		{CKA_WRAP, &true, sizeof(true)},
		{CKA_MODULUS_BITS, &modulusBits, sizeof(modulusBits)},
		{CKA_PUBLIC_EXPONENT, publicExponent, sizeof(publicExponent)}
	};
	CK_ULONG privateKeyTemplateAttributeCountRSA = 8;
	CK_ATTRIBUTE privateKeyTemplateRSA[] = {
		{CKA_TOKEN, &true, sizeof(true)},
		{CKA_PRIVATE, &true, sizeof(true)},
		{CKA_SUBJECT, subject, sizeof(subject)},
		{CKA_ID, id, sizeof(id)},
		{CKA_SENSITIVE, &true, sizeof(true)},
		{CKA_DECRYPT, &true, sizeof(true)},
		{CKA_SIGN, &true, sizeof(true)},
		{CKA_UNWRAP, &true, sizeof(true)}
	};	
		
		
	printf("Testing case: KeyPairGeneration ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseKeyPairGenerationDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
		
		for(j=0;j<pMechaInfoListLen;j++)//each mechanism
		{
			if(!(pMechaInfoList[j].info.flags & CKF_GENERATE_KEY_PAIR)) continue; //that is capable of generating
			mechanism.mechanism = pMechaInfoList[j].type;
			
			k = searchEleName(mechanism.mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));

			
			rv = setTemplateForKeyPair(&mechanism, &publicKeyTemplate, &publicKeyAttributeCount, &privateKeyTemplate, &privateKeyAttributeCount);
			if(rv == 0)
			{
				//C_GenerateKeyPair
				if(k != -1)	printf(" call C_GenerateKeyPair(%s) on slot %lu ...", MECHANISM_LIST[k].eleName, slotID);
				else printf(" call C_GenerateKeyPair(mecha = %lu) on slot %lu ...", mechanism.mechanism, slotID);
				//C_GenerateKeyPair
				rv = pFunctionList->C_GenerateKeyPair(hSession, &mechanism, 
														publicKeyTemplateRSA, publicKeyTemplateAttributeCountRSA,
														privateKeyTemplateRSA, privateKeyTemplateAttributeCountRSA,
														&hPublicKey, &hPrivateKey);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else
				{
					 printf(" success\n");
					if(checkFunctionImplemented("C_DestroyObject")!=-1)
					{
						pFunctionList->C_DestroyObject(hSession, hPublicKey);
						pFunctionList->C_DestroyObject(hSession, hPrivateKey);
					}
				}

			}
			else
			{
				if(k != -1)	printf(" skip C_GenerateKeyPair(%s) because mecha is not yet implemented ...\n", MECHANISM_LIST[k].eleName);
				else printf(" skip C_GenerateKeyPair(mecha = %lu) because mecha is not yet implemented ...\n", mechanism.mechanism);
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
		
		closeSession(hSession);	
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
