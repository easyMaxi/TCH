int
testFunctionsImplemented()
{
	CK_SESSION_HANDLE hSession = 0;
	CK_BYTE OperationState = 'a';
	CK_INFO info;
	CK_ULONG uLong = 0;
	CK_RV rv;
	CK_SLOT_ID slotID = 0;
	CK_SLOT_INFO slotInfo;
	CK_TOKEN_INFO tokenInfo;
	CK_BBOOL tokenPresent = 42;//so we do nothing on a real token
	CK_MECHANISM_TYPE mechaType = 0;
	CK_MECHANISM_INFO mechaInfo;
	CK_UTF8CHAR utfChar;
	const char *utfChar2 = "TestP";
	CK_OBJECT_HANDLE hObjectReal = 0; //don't use it for reading purpose. it could be valid
	CK_OBJECT_HANDLE hObject = 0;
	CK_ATTRIBUTE_PTR pAttribute = NULL_PTR;
	CK_MECHANISM mecha;
	CK_BYTE_PTR exampleData;
	generateSampleData(64, &exampleData);
	

	printf("Testing implementation of functions ... \n");

	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Initialize");
		printf(" C_Initialize: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Initialize: Not supported.\n");

	rv = pFunctionList->C_GetInfo(&info);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetInfo");
		printf(" C_GetInfo: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetInfo: Not supported.\n");

	rv = pFunctionList->C_GetSlotList(slotID, NULL_PTR, &uLong);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetSlotList");
		printf(" C_GetSlotList: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetSlotList: Not supported.\n");

	rv = pFunctionList->C_GetSlotInfo(slotID, &slotInfo);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetSlotInfo");
		printf(" C_GetSlotInfo: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetSlotInfo: Not supported.\n");

	rv = pFunctionList->C_GetTokenInfo(slotID, &tokenInfo);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetTokenInfo");
		printf(" C_GetTokenInfo: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetTokenInfo: Not supported.\n");

	rv = pFunctionList->C_WaitForSlotEvent(CKF_DONT_BLOCK, &slotID, NULL_PTR);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_WaitForSlotEvent");
		printf(" C_WaitForSlotEvent(CKF_DONT_BLOCK): Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_WaitForSlotEvent(CKF_DONT_BLOCK): Not supported.\n");

	rv = pFunctionList->C_GetMechanismList(tokenPresent, NULL_PTR, &uLong);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetMechanismList");
		printf(" C_GetMechanismList: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetMechanismList: Not supported.\n");

	rv = pFunctionList->C_GetMechanismInfo(tokenPresent, mechaType, &mechaInfo);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetMechanismInfo");
		printf(" C_GetMechanismInfo: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetMechanismInfo: Not supported.\n");

	rv = pFunctionList->C_InitToken(tokenPresent, &utfChar, uLong, &utfChar);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_InitToken");
		printf(" C_InitToken: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_InitToken: Not supported.\n");

	rv = pFunctionList->C_InitPIN(hSession, &utfChar, uLong);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_InitPIN");
		printf(" C_InitPIN: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_InitPIN: Not supported.\n");

	rv = pFunctionList->C_SetPIN(hSession, &utfChar, uLong, &utfChar, uLong);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SetPIN");
		printf(" C_SetPIN: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SetPIN: Not supported.\n");

	rv = pFunctionList->C_OpenSession(0, 0, NULL_PTR, NULL_PTR, &hSession);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_OpenSession");
		printf(" C_OpenSession: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_OpenSession: Not supported.\n");

	rv = pFunctionList->C_CloseSession(0);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_CloseSession");
		printf(" C_CloseSession: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_CloseSession: Not supported.\n");

	rv = pFunctionList->C_CloseAllSessions(0);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_CloseAllSessions");
		printf(" C_CloseAllSessions: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_CloseAllSessions: Not supported.\n");

	rv = pFunctionList->C_GetSessionInfo(hSession, NULL_PTR);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetSessionInfo");
		printf(" C_GetSessionInfo: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetSessionInfo: Not supported.\n");

	rv = pFunctionList->C_GetOperationState(hSession, &OperationState, &uLong);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetOperationState");
		printf(" C_GetOperationState: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetOperationState: Not supported.\n");

	rv = pFunctionList->C_SetOperationState(hSession, &OperationState, sizeof(OperationState), hObject, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SetOperationState");
		printf(" C_SetOperationState: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SetOperationState: Not supported.\n");

	rv = pFunctionList->C_Login(0, 0, (CK_UTF8CHAR_PTR) utfChar2, strlen((char *)utfChar2)); //@todo erwähnen, dass der stick probleme mit ungültigen usern hat
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Login");
		printf(" C_Login: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Login: Not supported.\n");

	rv = pFunctionList->C_Logout(hSession); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Logout");
		printf(" C_Logout: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Logout: Not supported.\n");

	rv = pFunctionList->C_CreateObject(hSession, pAttribute, 0, &hObjectReal); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_CreateObject");
		printf(" C_CreateObject: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_CreateObject: Not supported.\n");

	rv = pFunctionList->C_CopyObject(hSession, hObject, pAttribute, 0, &hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_CopyObject");
		printf(" C_CopyObject: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_CopyObject: Not supported.\n");

	rv = pFunctionList->C_DestroyObject(hSession, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DestroyObject");
		printf(" C_DestroyObject: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DestroyObject: Not supported.\n");

	rv = pFunctionList->C_GetObjectSize(hSession, hObject, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetObjectSize");
		printf(" C_GetObjectSize: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetObjectSize: Not supported.\n");

	rv = pFunctionList->C_GetAttributeValue(hSession, hObject, pAttribute, uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GetAttributeValue");
		printf(" C_GetAttributeValue: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GetAttributeValue: Not supported.\n");

	rv = pFunctionList->C_SetAttributeValue(hSession, hObject, pAttribute, uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SetAttributeValue");
		printf(" C_SetAttributeValue: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SetAttributeValue: Not supported.\n");

	rv = pFunctionList->C_FindObjectsInit(hSession, pAttribute, uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_FindObjectsInit");
		printf(" C_FindObjectsInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_FindObjectsInit: Not supported.\n");

	rv = pFunctionList->C_FindObjects(hSession, &hObject, (CK_ULONG) 0, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_FindObjects");
		printf(" C_FindObjects: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_FindObjects: Not supported.\n");

	rv = pFunctionList->C_FindObjectsFinal(hSession); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_FindObjectsFinal");
		printf(" C_FindObjectsFinal: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_FindObjectsFinal: Not supported.\n");

	rv = pFunctionList->C_EncryptInit(hSession, &mecha, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_EncryptInit");
		printf(" C_EncryptInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_EncryptInit: Not supported.\n");

	rv = pFunctionList->C_Encrypt(hSession, &OperationState, 0, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Encrypt");
		printf(" C_Encrypt: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Encrypt: Not supported.\n");

	rv = pFunctionList->C_EncryptUpdate(hSession, &OperationState, 0, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_EncryptUpdate");
		printf(" C_EncryptUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_EncryptUpdate: Not supported.\n");

	rv = pFunctionList->C_EncryptFinal(hSession, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_EncryptFinal");
		printf(" C_EncryptFinal: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_EncryptFinal: Not supported.\n");

	rv = pFunctionList->C_DecryptInit(hSession, &mecha, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DecryptInit");
		printf(" C_DecryptInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DecryptInit: Not supported.\n");

	rv = pFunctionList->C_Decrypt(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Decrypt");
		printf(" C_Decrypt: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Decrypt: Not supported.\n");

	rv = pFunctionList->C_DecryptUpdate(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DecryptUpdate");
		printf(" C_DecryptUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DecryptUpdate: Not supported.\n");

	rv = pFunctionList->C_DecryptFinal(hSession, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DecryptFinal");
		printf(" C_DecryptFinal: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DecryptFinal: Not supported.\n");

	rv = pFunctionList->C_DigestInit(hSession, &mecha); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DigestInit");
		printf(" C_DigestInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DigestInit: Not supported.\n");

	rv = pFunctionList->C_Digest(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Digest");
		printf(" C_Digest: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Digest: Not supported.\n");

	rv = pFunctionList->C_DigestUpdate(hSession, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DigestUpdate");
		printf(" C_DigestUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DigestUpdate: Not supported.\n");

	rv = pFunctionList->C_DigestKey(hSession, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DigestKey");
		printf(" C_DigestKey: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DigestKey: Not supported.\n");

	rv = pFunctionList->C_DigestFinal(hSession, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DigestFinal");
		printf(" C_DigestFinal: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DigestFinal: Not supported.\n");

	rv = pFunctionList->C_SignInit(hSession, &mecha, OperationState); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SignInit");
		printf(" C_SignInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SignInit: Not supported.\n");

	//rv = pFunctionList->C_Sign(0, exampleData, 64, NULL_PTR, &uLong);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Sign");
		printf(" C_Sign: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Sign: Not supported.\n");

	rv = pFunctionList->C_SignUpdate(hSession, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SignUpdate");
		printf(" C_SignUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SignUpdate: Not supported.\n");

	//rv = pFunctionList->C_SignFinal(hSession, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SignFinal");
		printf(" C_SignFinal: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SignFinal: Not supported.\n");

	rv = pFunctionList->C_SignRecoverInit(hSession, &mecha, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SignRecoverInit");
		printf(" C_SignRecoverInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SignRecoverInit: Not supported.\n");

	//rv = pFunctionList->C_SignRecover(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SignRecover");
		printf(" C_SignRecover: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SignRecover: Not supported.\n");

	rv = pFunctionList->C_VerifyInit(hSession, &mecha, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_VerifyInit");
		printf(" C_VerifyInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_VerifyInit: Not supported.\n");

	rv = pFunctionList->C_Verify(hSession, &OperationState, 0, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Verify");
		printf(" C_Verify: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Verify: Not supported.\n");

	rv = pFunctionList->C_VerifyUpdate(hSession, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_VerifyUpdate");
		printf(" C_VerifyUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_VerifyUpdate: Not supported.\n");

	rv = pFunctionList->C_VerifyFinal(hSession, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_VerifyFinal");
		printf(" C_VerifyFinal: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_VerifyFinal: Not supported.\n");

	rv = pFunctionList->C_VerifyRecoverInit(hSession, &mecha, hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_VerifyRecoverInit");
		printf(" C_VerifyRecoverInit: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_VerifyRecoverInit: Not supported.\n");

	rv = pFunctionList->C_VerifyRecover(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_VerifyRecover");
		printf(" C_VerifyRecover: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_VerifyRecover: Not supported.\n");

	rv = pFunctionList->C_DigestEncryptUpdate(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DigestEncryptUpdate");
		printf(" C_DigestEncryptUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DigestEncryptUpdate: Not supported.\n");

	rv = pFunctionList->C_DecryptDigestUpdate(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DecryptDigestUpdate");
		printf(" C_DecryptDigestUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DecryptDigestUpdate: Not supported.\n");

	rv = pFunctionList->C_SignEncryptUpdate(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SignEncryptUpdate");
		printf(" C_SignEncryptUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SignEncryptUpdate: Not supported.\n");

	rv = pFunctionList->C_DecryptVerifyUpdate(hSession, &OperationState, 0, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DecryptVerifyUpdate");
		printf(" C_DecryptVerifyUpdate: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DecryptVerifyUpdate: Not supported.\n");

	rv = pFunctionList->C_GenerateKey(hSession, &mecha, pAttribute, 0, &hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GenerateKey");
		printf(" C_GenerateKey: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GenerateKey: Not supported.\n");

	rv = pFunctionList->C_GenerateKeyPair(hSession, &mecha, pAttribute, 0, pAttribute, 0, &hObject, &hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GenerateKeyPair");
		printf(" C_GenerateKeyPair: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GenerateKeyPair: Not supported.\n");

	rv = pFunctionList->C_WrapKey(hSession, &mecha, hObjectReal, hObjectReal, &OperationState, &uLong); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_WrapKey");
		printf(" C_WrapKey: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_WrapKey: Not supported.\n");

	rv = pFunctionList->C_UnwrapKey(hSession, &mecha, hObjectReal, &OperationState, 0, pAttribute, 0, &hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_UnwrapKey");
		printf(" C_UnwrapKey: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_UnwrapKey: Not supported.\n");

	rv = pFunctionList->C_DeriveKey(hSession, &mecha, hObjectReal, pAttribute, 0, &hObject); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_DeriveKey");
		printf(" C_DeriveKey: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_DeriveKey: Not supported.\n");

	rv = pFunctionList->C_SeedRandom(hSession, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_SeedRandom");
		printf(" C_SeedRandom: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_SeedRandom: Not supported.\n");

	rv = pFunctionList->C_GenerateRandom(hSession, &OperationState, 0); 
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_GenerateRandom");
		printf(" C_GenerateRandom: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_GenerateRandom: Not supported.\n");

	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_FUNCTION_NOT_SUPPORTED)
	{
		setFunctionImplemented("C_Finalize");
		printf(" C_Finalize: Supported.\n");
	}
	if(rv == CKR_FUNCTION_NOT_SUPPORTED)printf(" C_Finalize: Not supported.\n");
	

		
	return 0;
}

