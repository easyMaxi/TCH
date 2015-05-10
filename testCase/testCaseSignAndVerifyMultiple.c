const char *testCaseSignAndVerifyMultipleDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_SignInit", "C_SignUpdate", "C_SignFinal", "C_VerifyInit", "C_VerifyUpdate", "C_VerifyFinal", NULL};

int
signMultiple(CK_BYTE_PTR data, CK_BYTE_PTR *dataSigned, CK_ULONG_PTR dataSignedLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR signMechanism)
{
	
	CK_RV rv;
	CK_BYTE_PTR dataFirstHalf, dataSecondHalf;
	CK_ULONG dataHalfLen;
	
	dataHalfLen = strlen((char *) data)/2;
	dataFirstHalf = data;
	dataSecondHalf = data + (dataHalfLen * sizeof(CK_BYTE));
	
	
	//SignInit
	rv = pFunctionList->C_SignInit(*hSession, signMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_SignInit", rv);
	}
	
	//C_SignUpdate 1/2
	rv = pFunctionList->C_SignUpdate(*hSession, dataFirstHalf, dataHalfLen);
	if(rv == CKR_KEY_FUNCTION_NOT_PERMITTED)
	{
		printf("  action is not supported by the key\n");
		return 1;
	}
	else if(rv != CKR_OK)
	{
		returnValuePrinting("C_SignUpdate", rv);
		return rv;
	}
	
	//C_SignUpdate 2/2
	rv = pFunctionList->C_SignUpdate(*hSession, dataSecondHalf, dataHalfLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_SignUpdate", rv);
		return rv;
	}

	
	//C_SignFinal
	rv = pFunctionList->C_SignFinal(*hSession, NULL_PTR, dataSignedLen);
	if(rv != CKR_OK)
	{
		returnTestCase(rv, 0);
		return rv;
	}
	
	*dataSigned = malloc((*dataSignedLen) * sizeof(CK_BYTE));
	
	rv = pFunctionList->C_SignFinal(*hSession, *dataSigned, dataSignedLen);
	if(rv != CKR_OK)
	{
		returnTestCase(rv, 0);
		return rv;
	}
	
	return 0;
}

int
verifyMultiple(CK_BYTE_PTR data, CK_ULONG dataLen, CK_BYTE_PTR signature, CK_ULONG signatureLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR mechanism)
{
	
	CK_RV rv;
	CK_BYTE_PTR dataFirstHalf, dataSecondHalf;
	CK_ULONG dataHalfLen;
	
	dataHalfLen = strlen((char *) data)/2;
	dataFirstHalf = data;
	dataSecondHalf = data + (dataHalfLen * sizeof(CK_BYTE));
	
	//VerifyInit
	rv = pFunctionList->C_VerifyInit(*hSession, mechanism, *hKey);
	if(rv == CKR_KEY_FUNCTION_NOT_PERMITTED)
	{
		printf("  action is not supported by the key\n");
		return 1;
	}
	else if(rv != CKR_OK)
	{
		returnValuePrinting("C_VerifyInit", rv);
		return rv;
	}
	
	//VerifyUpdate
	rv = pFunctionList->C_VerifyUpdate(*hSession, dataFirstHalf, dataHalfLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_VerifyUpdate", rv);
		return rv;
	}
	//VerifyUpdate
	rv = pFunctionList->C_VerifyUpdate(*hSession, dataSecondHalf, dataHalfLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_VerifyUpdate", rv);
		return rv;
	}
	
	//VerifyFinal
	rv = pFunctionList->C_VerifyFinal(*hSession, signature, signatureLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Verify", rv);
		return rv;
	}
	
	return rv;
}



int
testCaseSignAndVerifyMultiple()
{
	CK_RV rv;
	CK_ULONG ulTokenCount = 0;
	CK_SLOT_ID_PTR pTokenList = NULL_PTR;
	CK_SLOT_ID slotID;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_MECHANISM keyMechanism = {
		0, NULL_PTR, 0
	};
	CK_MECHANISM mechanism = {
		0, NULL_PTR, 0
	};
	CK_OBJECT_HANDLE hKey = 0, hPublicKey = 0;
	CK_BYTE_PTR data = NULL_PTR;
	CK_BYTE_PTR dataSigned = NULL_PTR;
	CK_ULONG dataSignedLen = 0;
	struct timeval start, end, diff;
	
	int i,k,j, temp;
		
	printf("Testing case: SignAndVerifyMultiple ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSignAndVerifyMultipleDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	initToken();
	
	//GetSlotList -> Tokenlist
	findActiveTokens(&pTokenList, &ulTokenCount);
	
	for(i=0;i<ulTokenCount;i++)
	{
		
		slotID = pTokenList[i];
	
		
		for(j=0;j<pMechaInfoListLen;j++)//each mechanism
		{
			if(!(pMechaInfoList[j].info.flags & (CKF_SIGN | CKF_VERIFY))) continue; //that is capable of encrypting + decrypting
			mechanism.mechanism = pMechaInfoList[j].type;
						
			k = searchEleName(mechanism.mechanism, MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
			if(k != -1)	printf(" Signing and Verify(%s) ...\n", MECHANISM_LIST[k].eleName);
			else printf("  Signing and Verify(mecha = %lu) ...\n", mechanism.mechanism);
						
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
					generateSampleData(16, &data);
					gettimeofday(&start, NULL);
					rv = signMultiple(data, &dataSigned, &dataSignedLen, &hKey, &hSession, &mechanism);
					gettimeofday(&end, NULL);
					if(rv == 0)
					{
						timeval_subtract(&diff, &end, &start);
						printf("   Could sign %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
						temp = dataSignedLen;
												
						gettimeofday(&start, NULL);
						if(hPublicKey != 0)	rv = verifyMultiple(data, strlen((char *) data), dataSigned, dataSignedLen, &hPublicKey, &hSession, &mechanism);//asymetric
						if(hPublicKey == 0)	rv = verifyMultiple(data, strlen((char *) data), dataSigned, dataSignedLen, &hKey, &hSession, &mechanism);//symetric
						gettimeofday(&end, NULL);
						timeval_subtract(&diff, &end, &start);
						if(rv == 0)
						{
							printf("   Could verify %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
							free(dataSigned);
						}
						else printf("    failed\n");
						free(data);
						
					
						//only test different data length if first one was successfull

						generateSampleData(256, &data);
						gettimeofday(&start, NULL);
						rv = signMultiple(data, &dataSigned, &dataSignedLen, &hKey, &hSession, &mechanism);
						gettimeofday(&end, NULL);
						if(rv == 0)
						{
							timeval_subtract(&diff, &end, &start);
							printf("   Could sign %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
							temp = dataSignedLen;
													
							gettimeofday(&start, NULL);
							if(hPublicKey != 0)	rv = verifyMultiple(data, strlen((char *) data), dataSigned, dataSignedLen, &hPublicKey, &hSession, &mechanism);//asymetric
							if(hPublicKey == 0)	rv = verifyMultiple(data, strlen((char *) data), dataSigned, dataSignedLen, &hKey, &hSession, &mechanism);//symetric
							gettimeofday(&end, NULL);
							timeval_subtract(&diff, &end, &start);
							if(rv == 0)
							{
								printf("   Could verify %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
								free(dataSigned);
							}
							else printf("    failed\n");
						}
						else printf("    failed\n");
						free(data);

												
						if(checkFunctionImplemented("C_DestroyObject")!=-1)
						{
							if(hPublicKey != 0) pFunctionList->C_DestroyObject(hSession, hPublicKey);
							if(hKey != 0)pFunctionList->C_DestroyObject(hSession, hKey);
						}
					}
					else printf("    failed\n");
				}
				else
				{
					if(k != -1)	printf("  skip %s because mecha is not yet implemented ...\n", MECHANISM_LIST[k].eleName);
					else printf("  skip mecha = %lu because mecha is not yet implemented ...\n", mechanism.mechanism);
				}
				logoutUser(hSession);			
				closeSession(hSession);
			}
			else
			{
				if(k != -1)	printf("  skip %s because the related key generation mechanism was not found...\n", MECHANISM_LIST[k].eleName);
				else printf("  skip mecha = %lu because the related key generation mechanism was not found...\n", mechanism.mechanism);
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
