const char *testCaseDeAndEncryptionSimpleDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_EncryptInit", "C_Encrypt", "C_DecryptInit", "C_Decrypt", NULL};

int
encrypt(CK_BYTE_PTR data, CK_BYTE_PTR *dataEnc, CK_ULONG_PTR resultLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR encryptMechanism)
{
	//resultLen is to return how long the encrypted data is
	CK_RV rv;

	
	//C_EncryptInit
	rv = pFunctionList->C_EncryptInit(*hSession, encryptMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_EncryptInit", rv);
		return rv;
	}
	
	//C_Encrypt NULL_PTR
	rv = pFunctionList->C_Encrypt(*hSession, data, strlen((char *)data), NULL_PTR, resultLen);
	if(rv != CKR_OK)
	{
		pFunctionList->C_EncryptFinal(*hSession, NULL, resultLen);
		returnValuePrinting("C_Encrypt", rv);
		return rv;
	}	
	*dataEnc = malloc(*resultLen * sizeof(CK_BYTE));

	//C_Encrypt
	rv = pFunctionList->C_Encrypt(*hSession, data, strlen((char *)data), *dataEnc, resultLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Encrypt", rv);
		return rv;
	}	
	
	return 0;
}

int
decrypt(CK_BYTE_PTR encData, CK_BYTE_PTR *dataDec, CK_ULONG_PTR resultLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR decryptMechanism)
{
	//resultLen is used to know how long the encrypted part is and give back how long the decrypted part is
	CK_RV rv;
	
	CK_ULONG dataDecLen = 0;

	
	//C_DecryptInit
	rv = pFunctionList->C_DecryptInit(*hSession, decryptMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_DecryptInit", rv);
		return rv;
	}
	
	//C_Decrypt NULL_PTR
	rv = pFunctionList->C_Decrypt(*hSession, encData, *resultLen, NULL_PTR, &dataDecLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Decrypt", rv);
		return rv;
	}	
	*dataDec = malloc(dataDecLen * sizeof(CK_BYTE));
		
	//C_Encrypt
	rv = pFunctionList->C_Decrypt(*hSession, encData, *resultLen, *dataDec, &dataDecLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Decrypt", rv);
		return rv;
	}	
	
	*resultLen = dataDecLen;
	
	return 0;
}

int
testCaseDeAndEncryptionSimple()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_BYTE_PTR data;
	CK_BYTE_PTR result = NULL_PTR;
	CK_ULONG resultLen = 0;
	int temp;//don't ask
	CK_MECHANISM keyMechanism = {
		0, NULL_PTR, 0
	};
	CK_MECHANISM mechanism = {
		0, NULL_PTR, 0
	};
	CK_OBJECT_HANDLE hKey = 0, hPublicKey = 0;
	struct timeval start, end, diff;

	int i,j,k;
		
	printf("Testing case: DeAndEncryptionSimple ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseDeAndEncryptionSimpleDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
				
		for(j=0;j<pMechaInfoListLen;j++)//each mechanism
		{
			if(!(pMechaInfoList[j].info.flags & (CKF_DECRYPT | CKF_ENCRYPT))) continue; //that is capable of encrypting + decrypting
			mechanism.mechanism = pMechaInfoList[j].type;

			k = searchEleName(mechanism.mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
			if(k != -1)	printf(" De - and encrypting(%s) ...\n", MECHANISM_LIST[k].eleName);
			else printf(" De - and encrypting(mecha = %lu) ...\n", mechanism.mechanism);
						
			rv = findGenerateMechanismForMechanism(&keyMechanism, mechanism.mechanism);
			if(rv == 0)
			{
				
				//OpenSession with CKF_SERIAL_SESSION
				printf(" call C_OpenSession on slot %lu with CKF_SERIAL_SESSION | CKF_RW_SESSION...", slotID);
				rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION | CKF_RW_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
				if(rv != CKR_OK) returnTestCase(rv, 0);
				else printf(" success\n");
				
				loginUser(hSession);
				
				hKey = 0;
				hPublicKey = 0;
				if((generateKey(&keyMechanism, &hKey, &hSession) == 0) | (generateKeyPair(&keyMechanism, &hKey, &hPublicKey, hSession) == 0))//gen key
				{
					
						printf("  encrypt + decrypt ...\n");

						generateSampleData(16, &data);
						gettimeofday(&start, NULL);					
						if(hPublicKey != 0) rv = encrypt(data, &result, &resultLen, &hPublicKey, &hSession, &mechanism);
						if(hPublicKey == 0) rv = encrypt(data, &result, &resultLen, &hKey, &hSession, &mechanism);
						gettimeofday(&end, NULL);
						if(rv == 0)
						{
							timeval_subtract(&diff, &end, &start);
							printf("   Could encrypt %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
							temp = resultLen;
							
							gettimeofday(&start, NULL);
							rv = decrypt(result, &result, &resultLen, &hKey, &hSession, &mechanism);
							gettimeofday(&end, NULL);
							timeval_subtract(&diff, &end, &start);
							if(rv == 0)	printf("   Could decrypt %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
							else printf("    failed\n");
						}
						else printf("    failed\n");
						free(data);
						
						generateSampleData(1048576, &data);
						gettimeofday(&start, NULL);;
						if(hPublicKey != 0) rv = encrypt(data, &result, &resultLen, &hPublicKey, &hSession, &mechanism);
						if(hPublicKey == 0) rv = encrypt(data, &result, &resultLen, &hKey, &hSession, &mechanism);
						gettimeofday(&end, NULL);
						if(rv == 0)
						{
							timeval_subtract(&diff, &end, &start);
							printf("   Could encrypt %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
							temp = resultLen;
													
							gettimeofday(&start, NULL);;
							rv = decrypt(result, &result, &resultLen, &hKey, &hSession, &mechanism);
							gettimeofday(&end, NULL);
							timeval_subtract(&diff, &end, &start);
							if(rv == 0)	printf("   Could decrypt %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
							else printf("    failed\n");
						}
						else printf("    failed\n");
						free(data);					
						

						
						generateSampleData(33554432, &data);
						gettimeofday(&start, NULL);;	
						if(hPublicKey != 0) rv = encrypt(data, &result, &resultLen, &hPublicKey, &hSession, &mechanism);
						if(hPublicKey == 0) rv = encrypt(data, &result, &resultLen, &hKey, &hSession, &mechanism);
						gettimeofday(&end, NULL);
						if(rv == 0)
						{
							timeval_subtract(&diff, &end, &start);
							printf("   Could encrypt %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
							temp = resultLen;
						
							gettimeofday(&start, NULL);;	
							rv = decrypt(result, &result, &resultLen, &hKey, &hSession, &mechanism);
							gettimeofday(&end, NULL);
							timeval_subtract(&diff, &end, &start);
							if(rv == 0)	printf("   Could decrypt %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
							else printf("    failed\n");
						}
						else printf("    failed\n");	
						free(data);			
						
						if(checkFunctionImplemented("C_DestroyObject")!=-1)
						{
							pFunctionList->C_DestroyObject(hSession, hPublicKey);
							pFunctionList->C_DestroyObject(hSession, hKey);
						}	

				}
				else
				{
					if(k != -1)	printf("  skip DeAndEncrypting(%s) because mecha is not yet implemented ...\n", MECHANISM_LIST[k].eleName);
					else printf("  skip DeAndEncrypting(mecha = %lu) because mecha is not yet implemented ...\n", mechanism.mechanism);
				}
				
				logoutUser(hSession);			
				closeSession(hSession);
			}
			else
			{
				if(k != -1)	printf("  skip DeAndEncrypting(%s) because the related key generation mechanism was not found...\n", MECHANISM_LIST[k].eleName);
				else printf("  skip DeAndEncrypting(mecha = %lu) because the related key generation mechanism was not found...\n", mechanism.mechanism);
			}
		}
					

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
