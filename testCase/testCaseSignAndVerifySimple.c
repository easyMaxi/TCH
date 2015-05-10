const char *testCaseSignAndVerifySimpleDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_SignInit", "C_Sign", "C_VerifyInit", "C_Verify", "C_GenerateKeyPair", NULL};

int
sign(CK_BYTE_PTR data, CK_BYTE_PTR *dataSigned, CK_ULONG_PTR dataSignedLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR signMechanism)
{
	
	CK_RV rv;
	
	//SignInit
	rv = pFunctionList->C_SignInit(*hSession, signMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_SignInit", rv);
		return rv;
	}
	
	//Sign NULL_PTR
	rv = pFunctionList->C_Sign(*hSession, data, strlen((char *)data), NULL_PTR, dataSignedLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Sign", rv);
		return rv;
	}


	*dataSigned = malloc((*dataSignedLen) * sizeof(CK_BYTE));
	
	//Sign
	rv = pFunctionList->C_Sign(*hSession, data, strlen((char *)data), *dataSigned, dataSignedLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Sign", rv);
		return rv;
	}
	
	return 0;
}

int
verify(CK_BYTE_PTR data, CK_ULONG dataLen, CK_BYTE_PTR signature, CK_ULONG signatureLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR mechanism)
{
	
	CK_RV rv;
	
	//VerifyInit
	rv = pFunctionList->C_VerifyInit(*hSession, mechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_VerifyInit", rv);
		return rv;
	}
	
	//Verify
	rv = pFunctionList->C_Verify(*hSession, data, dataLen, signature, signatureLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_Verify", rv);
		return rv;
	}
	
	return rv;
}



int
testCaseSignAndVerifySimple()
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
		
	printf("Testing case: SignAndVerifySimple ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseSignAndVerifySimpleDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
					rv = sign(data, &dataSigned, &dataSignedLen, &hKey, &hSession, &mechanism);
					gettimeofday(&end, NULL);
					if(rv == 0)
					{
						timeval_subtract(&diff, &end, &start);
						printf("   Could sign %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
						temp = dataSignedLen;
												
						gettimeofday(&start, NULL);
						if(hPublicKey != 0)	rv = verify(data, strlen((char *) data), dataSigned, dataSignedLen, &hPublicKey, &hSession, &mechanism);//asymetric
						if(hPublicKey == 0)	rv = verify(data, strlen((char *) data), dataSigned, dataSignedLen, &hKey, &hSession, &mechanism);//symetric
						gettimeofday(&end, NULL);
						timeval_subtract(&diff, &end, &start);
						if(rv == 0)
						{
							printf("   Could verify %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
							free(dataSigned);
						}
						else printf("    failed\n");
						free(data);
						
						//try different data length

						generateSampleData(256, &data);
						gettimeofday(&start, NULL);
						rv = sign(data, &dataSigned, &dataSignedLen, &hKey, &hSession, &mechanism);
						gettimeofday(&end, NULL);
						if(rv == 0)
						{
							timeval_subtract(&diff, &end, &start);
							printf("   Could sign %zu CK_BYTEs in %ld.%06ld sec.\n", strlen((char *) data), diff.tv_sec, diff.tv_usec);
							temp = dataSignedLen;
													
							gettimeofday(&start, NULL);
							if(hPublicKey != 0)	rv = verify(data, strlen((char *) data), dataSigned, dataSignedLen, &hPublicKey, &hSession, &mechanism);//asymetric
							if(hPublicKey == 0)	rv = verify(data, strlen((char *) data), dataSigned, dataSignedLen, &hKey, &hSession, &mechanism);//symetric
							gettimeofday(&end, NULL);
							timeval_subtract(&diff, &end, &start);
							if(rv == 0)
							{
								printf("   Could verify %d CK_BYTEs in %ld.%06ld sec.\n", temp, diff.tv_sec, diff.tv_usec);
							}	
							else printf("    failed\n");
							free(dataSigned);
						}
						else printf("    failed\n");
						free(data);
					}
					else printf("    failed\n");
					
											
					if(checkFunctionImplemented("C_DestroyObject")!=-1)
					{
						pFunctionList->C_DestroyObject(hSession, hPublicKey);
						pFunctionList->C_DestroyObject(hSession, hKey);
					}	
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
	finalizeToken();
	
	free(pTokenList);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
