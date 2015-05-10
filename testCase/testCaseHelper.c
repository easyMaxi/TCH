
//function to reduce the ->Finalize lines
int
returnTestCase(int rv, int exit)
{
	if(exit) pFunctionList->C_Finalize(NULL_PTR);
	returnValuePrinting("", rv);
	return rv;
}

int
findActiveTokens(CK_SLOT_ID_PTR *pTokenList, CK_ULONG_PTR ulTokenCount)
{
	CK_RV rv;
	
	printf(" call C_GetSlotList with CK_TRUE and NULL_PTR ...");
	rv = pFunctionList->C_GetSlotList(CK_TRUE, NULL_PTR, ulTokenCount); //we need the number of tokens
	if(rv != CKR_OK) returnTestCase(rv, 0);	
	else printf(" success\n");
	
	*pTokenList = malloc((*ulTokenCount) * sizeof(CK_SLOT_ID)); //make a list
	
	printf(" call C_GetSlotList with CK_TRUE ...");
	rv = pFunctionList->C_GetSlotList(CK_TRUE, *pTokenList, ulTokenCount); //and fill the list
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	return 0;
}

int
initToken()
{
	CK_RV rv;
	
	printf(" call Initialize ...");
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");	
	
	return 0;
}

int
finalizeToken()
{
	CK_RV rv;
	
	printf(" call Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	return 0;
}

int
loginUser(CK_SESSION_HANDLE hSession)
{
	CK_RV rv;
	
	if(sArguments.uPin)
	{
		//Login
		printf(" call C_Login as User ...");
		rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
		if(rv != CKR_OK)
		{
			returnTestCase(rv, 1);
			exit(EXIT_FAILURE);
		}
		else printf(" success\n");	
	}
	
	return 0;
}

int
logoutUser(CK_SESSION_HANDLE hSession)
{
	CK_RV rv;
	
	if(sArguments.uPin)
	{
		//Logout
		printf(" call C_Logout ...");
		rv = pFunctionList->C_Logout(hSession);
		if(rv != CKR_OK) returnTestCase(rv, 0);
		else printf(" success\n");		
	}
	
	return 0;
}

int
closeSession(CK_SESSION_HANDLE hSession)
{
	CK_RV rv;
	
	//CloseSession
	printf(" call C_CloseSession with the previous session handle...");
	rv = pFunctionList->C_CloseSession(hSession);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");	
	
	return 0;
}

int
getTokenInfo(CK_SLOT_ID slotID, CK_TOKEN_INFO_PTR tokenInfo)
{
	CK_RV rv;
	
	//GetTokenInfo
	printf(" call C_GetTokenInfo on slot %lu ...", slotID);
	rv = pFunctionList->C_GetTokenInfo(slotID, tokenInfo);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	return 0;
}

int
generateSampleData(int length, CK_BYTE_PTR *data)
{
	
	int i, setKey;
	const char set[] = "abcdefghijklmnopqrstuvwxyz";
	
	*data = malloc((length * sizeof(CK_BYTE)) + 1);
	
	for(i=0;i<length;i++)
	{
		setKey = rand() % (int) (strlen(set));
		(*data)[i] = (CK_BYTE) set[setKey];
	}
	(*data)[length]='\0';

    return 0;
}

int
generateObject(CK_SLOT_ID slotID, CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE_PTR hObject, CK_ATTRIBUTE_PTR objectTemplate, CK_ULONG templateSize)
{
	CK_RV rv;
	
	printf(" call C_CreateObject with object class %lu...", *((CK_ULONG *) objectTemplate->pValue));
	rv = pFunctionList->C_CreateObject(hSession, objectTemplate, templateSize, hObject);
	if(rv != CKR_OK)
	{
		returnTestCase(rv, 0);
		return 1;
	}
	else printf(" success\n");
	
	return 0;
}

int
setDigestParams(CK_MECHANISM_PTR mechanism)
{
	switch(mechanism->mechanism)
	{
		case CKM_SHA_1:
		case CKM_MD5:
		case CKM_SHA256:
		case CKM_SHA384:
		case CKM_SHA512:
			mechanism->pParameter = NULL_PTR;
			mechanism->ulParameterLen = 0;
			break;
		default:
			return 1;
			break;
	}
	return 0;
}


int setTemplateForKeyPair(CK_MECHANISM_PTR mechanism, CK_ATTRIBUTE_PTR *pPublicKeyTemplate, CK_ULONG_PTR publicKeyTemplateAttributeCount, CK_ATTRIBUTE_PTR *pPrivateKeyTemplate, CK_ULONG_PTR privateKeyTemplateAttributeCount)
{
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
	
	switch(mechanism->mechanism)
	{
		case CKM_RSA_PKCS_KEY_PAIR_GEN:
			mechanism->pParameter = NULL_PTR;
			mechanism->ulParameterLen = 0;
			*pPublicKeyTemplate = publicKeyTemplateRSA;
			*publicKeyTemplateAttributeCount = publicKeyTemplateAttributeCountRSA;
			*pPrivateKeyTemplate = privateKeyTemplateRSA;
			*privateKeyTemplateAttributeCount = privateKeyTemplateAttributeCountRSA;
			break;
		default:
			return 1;
			break;
	}
	return 0;
}

int setParamsForKey(CK_MECHANISM_PTR mechanism, CK_ATTRIBUTE_PTR *template, CK_ULONG_PTR templateSize)
{
	CK_BBOOL true = CK_TRUE;
	CK_ULONG ulKeyLen = 32; 
		
	CK_ATTRIBUTE pTemplateAES[] = {
		{ CKA_TOKEN, &true, sizeof(true) },
		{ CKA_PRIVATE, &true, sizeof(true) },
		{ CKA_VALUE_LEN, &ulKeyLen, sizeof(ulKeyLen) },
		{ CKA_ENCRYPT, &true, sizeof(true)  }
	};
	CK_ULONG ulCountAES = 4;
	
	
	switch(mechanism->mechanism)
	{
		case CKM_DES_KEY_GEN:
		case CKM_DES2_KEY_GEN:
		case CKM_DES3_KEY_GEN:
			mechanism->pParameter = NULL_PTR;
			mechanism->ulParameterLen = 0;
			break;
		case CKM_AES_KEY_GEN: //seems not to work. needs further testing
			mechanism->pParameter = NULL_PTR;
			mechanism->ulParameterLen = 0;
			*template = pTemplateAES;
			*templateSize = ulCountAES;
			break;
		default:
			return 1;
			break;
	}
	return 0;
}

int
generateKey(CK_MECHANISM_PTR mechanism, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession)
{		
	CK_RV rv;
	CK_ATTRIBUTE_PTR template = NULL_PTR;
	CK_ULONG templateSize = 0;
	
	if(checkFunctionImplemented("C_GenerateKey") == -1) return 1;
		
	rv = setParamsForKey(mechanism, &template, &templateSize);
	if(rv == 0)
	{
		//GenerateKey
		rv = pFunctionList->C_GenerateKey(*hSession, mechanism, template, templateSize, hKey);
		if(rv != CKR_OK)
		{
			returnValuePrinting("C_GenerateKey", rv);
			return 1;
		}

	}
	else
	{
		return 1;
	}
	

	return 0;
}

int generateKeyPair(CK_MECHANISM_PTR keyMechanism, CK_OBJECT_HANDLE_PTR privateKey, CK_OBJECT_HANDLE_PTR publicKey, CK_SESSION_HANDLE hSession)
{
		
	CK_RV rv;
	if(checkFunctionImplemented("C_GenerateKeyPair") == -1) return 1;


	//rv = setTemplateForKeyPair(keyMechanism, &publicKeyTemplate, &publicKeyAttributeCount, &privateKeyTemplate, &privateKeyAttributeCount);
	
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
	
	//if(rv == 0)
	if(keyMechanism->mechanism == CKM_RSA_PKCS_KEY_PAIR_GEN)
	{
		//C_GenerateKeyPair
		rv = pFunctionList->C_GenerateKeyPair(hSession, keyMechanism, 
												publicKeyTemplateRSA, publicKeyTemplateAttributeCountRSA,
												privateKeyTemplateRSA, privateKeyTemplateAttributeCountRSA,
												publicKey, privateKey);
		if(rv != CKR_OK) returnValuePrinting("C_GenerateKeyPair", rv);
	}
	else
	{
		return 1;
	}
	return 0;
}

int
findGenerateMechanismForMechanism(CK_MECHANISM_PTR keyMecha, CK_MECHANISM_TYPE mecha)
{
	switch(mecha)
	{
		case CKM_DES_ECB:
		case CKM_DES_CBC:
		case CKM_DES_MAC:
		case CKM_DES_MAC_GENERAL:
			keyMecha->mechanism = CKM_DES_KEY_GEN;
			break;
		case CKM_DES3_ECB:
		case CKM_DES3_CBC:
		case CKM_DES3_MAC:
		case CKM_DES3_MAC_GENERAL:
			keyMecha->mechanism = CKM_DES3_KEY_GEN;
			break;
		case CKM_AES_ECB:
		case CKM_AES_CBC:
		case CKM_AES_MAC:
		case CKM_AES_MAC_GENERAL:
			keyMecha->mechanism = CKM_AES_KEY_GEN;
			break;		
		case CKM_RSA_PKCS:
		case CKM_RSA_X_509:
		case CKM_SHA256_RSA_PKCS:
		case CKM_MD5_RSA_PKCS:
		case CKM_SHA1_RSA_PKCS:
		case CKM_SHA384_RSA_PKCS:
		case CKM_SHA512_RSA_PKCS:
			keyMecha->mechanism = CKM_RSA_PKCS_KEY_PAIR_GEN;
			break;
		case CKM_SHA256_HMAC: //standard says "generic secrete key" ... so  we take one
		case CKM_SHA384_HMAC:
		case CKM_SHA512_HMAC:
			keyMecha->mechanism = CKM_AES_KEY_GEN;
			break;
		default:
			return 1;
			break;
	}
	
	return 0;
}

//http://stackoverflow.com/questions/1468596/calculating-elapsed-time-in-a-c-program-in-milliseconds
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}
