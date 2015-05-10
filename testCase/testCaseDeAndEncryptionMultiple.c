const char *testCaseDeAndEncryptionMultipleDependencies[] = {"C_Initialize", "C_Finalize", "C_GetSlotList", "C_EncryptInit", "C_EncryptUpdate", "C_EncryptFinal","C_DecryptInit", "C_DecryptUpdate", "C_DecryptFinal", NULL};


int
encryptMultiple(CK_BYTE_PTR data, CK_BYTE_PTR *dataEnc, CK_ULONG_PTR resultLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR encryptMechanism)
{
	//resultLen is to return how long the encrypted data is
	CK_RV rv;
	CK_ULONG firstHalfEncLen, secondHalfEncLen, finishLen = 0;
	CK_ULONG dataHalf = strlen((char * )data) / 2;
	CK_BYTE_PTR dataFirstHalf, dataEncFirstHalf;
	CK_BYTE_PTR dataSecondHalf, dataEncSecHalf;
	CK_BYTE_PTR finishEncPart;
	
	dataFirstHalf = data; //first half of the string
	dataSecondHalf = data + (dataHalf * sizeof(CK_BYTE)); //second half of the string	
	
	
	//C_EncryptInit
	rv = pFunctionList->C_EncryptInit(*hSession, encryptMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_EncryptInit", rv);
		return rv;
	}
	
	//C_EncryptUpdate NULL_PTR 1/2
	rv = pFunctionList->C_EncryptUpdate(*hSession, dataFirstHalf, dataHalf, NULL_PTR, &firstHalfEncLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_EncryptUpdate NULL_PTR", rv);
		return rv;
	}	
	
	dataEncFirstHalf = malloc(firstHalfEncLen * sizeof(CK_BYTE));

	//C_EncryptUpdate 1/2
	rv = pFunctionList->C_EncryptUpdate(*hSession, dataFirstHalf, dataHalf, dataEncFirstHalf, &firstHalfEncLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_EncryptUpdate", rv);
		return rv;
	}	
	
	//C_EncryptUpdate NULL_PTR 2/2
	rv = pFunctionList->C_EncryptUpdate(*hSession, dataSecondHalf, strlen((char *) dataSecondHalf), NULL_PTR, &secondHalfEncLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_EncryptUpdate NULL_PTR", rv);
		return rv;
	}	
	dataEncSecHalf = malloc(secondHalfEncLen * sizeof(CK_BYTE));

	//C_EncryptUpdate 2/2
	rv = pFunctionList->C_EncryptUpdate(*hSession, dataSecondHalf, strlen((char *)dataSecondHalf), dataEncSecHalf, &secondHalfEncLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_EncryptUpdate", rv);
		return rv;
	}	
	
	//EncryptFinal
	rv = pFunctionList->C_EncryptFinal(*hSession, NULL_PTR, &finishLen);
	if((rv != CKR_OK))
	{
		returnValuePrinting("C_EncryptFinal", rv);
		return rv;
	}	
	
	finishEncPart = malloc(finishLen * sizeof(CK_BYTE));
	rv = pFunctionList->C_EncryptFinal(*hSession, finishEncPart, &finishLen);
	if((rv != CKR_OK))
	{
		returnValuePrinting("C_EncryptFinal", rv);
		return rv;
	}	

	
	*resultLen = secondHalfEncLen+firstHalfEncLen;
	*dataEnc = malloc(*resultLen * sizeof(CK_BYTE));
	memcpy(*dataEnc, dataEncFirstHalf, firstHalfEncLen);
	memcpy(*dataEnc+(firstHalfEncLen*sizeof(CK_BYTE)), dataEncSecHalf, secondHalfEncLen);
	
	free(dataEncFirstHalf);
	free(dataEncSecHalf);
	free(finishEncPart);
	return 0;
}

int
decryptMultiple(char *encData, CK_BYTE_PTR *dataDec, CK_ULONG_PTR resultLen, CK_OBJECT_HANDLE_PTR hKey, CK_SESSION_HANDLE_PTR hSession, CK_MECHANISM_PTR decryptMechanism)
{
	//resultLen is used to know how long the encrypted part is and give back how long the decrypted part is
	CK_RV rv;
	CK_ULONG firstHalfDecLen, secondHalfDecLen, finishLen = 0;
	CK_ULONG encDataHalfLen = *resultLen / 2;
	CK_BYTE_PTR dataEncFirstHalf, dataDecFirstHalf;
	CK_BYTE_PTR dataEncSecondHalf, dataDecSecHalf;
	CK_BYTE_PTR finishDecPart;
	
	dataEncFirstHalf = (CK_BYTE_PTR) encData; //first half of the string
	dataEncSecondHalf = (CK_BYTE_PTR) encData + (encDataHalfLen * sizeof(CK_BYTE)); //second half of the string	
	
	
	//C_DecryptInit
	rv = pFunctionList->C_DecryptInit(*hSession, decryptMechanism, *hKey);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_DecryptInit", rv);
		return rv;
	}
	
	//C_DecryptUpdate NULL_PTR 1/2
	rv = pFunctionList->C_DecryptUpdate(*hSession, dataEncFirstHalf, encDataHalfLen, NULL_PTR, &firstHalfDecLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_DecryptUpdate", rv);
		return rv;
	}	
	dataDecFirstHalf = malloc(firstHalfDecLen * sizeof(CK_BYTE));

	//C_DecryptUpdate 1/2
	rv = pFunctionList->C_DecryptUpdate(*hSession, dataEncFirstHalf, encDataHalfLen, dataDecFirstHalf, &firstHalfDecLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_DecryptUpdate", rv);
		return rv;
	}	
	
	//C_DecryptUpdate NULL_PTR 2/2
	rv = pFunctionList->C_DecryptUpdate(*hSession, dataEncSecondHalf, encDataHalfLen, NULL_PTR, &secondHalfDecLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_DecryptUpdate", rv);
		return rv;
	}	
	dataDecSecHalf = malloc(secondHalfDecLen * sizeof(CK_BYTE));

	//C_DecryptUpdate 2/2
	rv = pFunctionList->C_DecryptUpdate(*hSession, dataEncSecondHalf, encDataHalfLen, dataDecSecHalf, &secondHalfDecLen);
	if(rv != CKR_OK)
	{
		returnValuePrinting("C_DecryptUpdate", rv);
		return rv;
	}	
	
	//DecryptFinal
	rv = pFunctionList->C_DecryptFinal(*hSession, NULL_PTR, &finishLen);
	if((rv != CKR_OK))
	{
		returnValuePrinting("C_DecryptFinal", rv);
		return rv;
	}	
	
	finishDecPart = malloc(finishLen * sizeof(CK_BYTE));
	rv = pFunctionList->C_DecryptFinal(*hSession, finishDecPart, &finishLen);
	if((rv != CKR_OK))
	{
		returnValuePrinting("C_DecryptFinal", rv);
		return rv;
	}	

	
	*resultLen = firstHalfDecLen+secondHalfDecLen;
	*dataDec = malloc(*resultLen * sizeof(CK_BYTE));
	memcpy(*dataDec, dataDecFirstHalf, firstHalfDecLen);
	memcpy(*dataDec+(firstHalfDecLen*sizeof(CK_BYTE)), dataDecSecHalf, secondHalfDecLen);
	
	free(dataDecFirstHalf);
	free(dataDecSecHalf);
	free(finishDecPart);
	return 0;
}

int
testCaseDeAndEncryptionMultiple()
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
	CK_OBJECT_HANDLE hKey = 0,hPublicKey = 0;
	struct timeval start, end, diff;

	
	int i,j,k;
		
	printf("Testing case: DeAndEncryptionMultiple ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseDeAndEncryptionMultipleDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
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
						if(hPublicKey != 0) rv = encryptMultiple(data, &result, &resultLen, &hPublicKey, &hSession, &mechanism);
						if(hPublicKey == 0) rv = encryptMultiple(data, &result, &resultLen, &hKey, &hSession, &mechanism);		
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
							else
							{
								printf("    failed\n");
								finalizeToken();
							}
							
							generateSampleData(1048576, &data);
							gettimeofday(&start, NULL);	
							if(hPublicKey != 0) rv = encryptMultiple(data, &result, &resultLen, &hPublicKey, &hSession, &mechanism);
							if(hPublicKey == 0) rv = encryptMultiple(data, &result, &resultLen, &hKey, &hSession, &mechanism);
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
								else
								{
									printf("    failed\n");
								}
							}
							else
							{
								printf("    failed\n");
							}	
							free(data);	
							
							generateSampleData(33554432, &data);
							gettimeofday(&start, NULL);	
							if(hPublicKey != 0) rv = encryptMultiple(data, &result, &resultLen, &hPublicKey, &hSession, &mechanism);
							if(hPublicKey == 0) rv = encryptMultiple(data, &result, &resultLen, &hKey, &hSession, &mechanism);
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
								else
								{
									printf("    failed\n");
									finalizeToken();
								}
							}
							else
							{
								printf("    failed\n");
								finalizeToken();
							}	
							free(data);		
						}
						else
						{
							printf("    failed\n");
						}
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
				if(k != -1)	printf("  skip DeAndEncrypting(%s) because related key generation mecha was not found ...\n", MECHANISM_LIST[k].eleName);
				else printf("  skip DeAndEncrypting(mecha = %lu) because related key generation mecha was not found ...\n", mechanism.mechanism);
			}
		}
					

		logoutUser(hSession);
	}
	
	//finalize
	finalizeToken();
	
	free(pTokenList);

	printf(" ... case successful\n");
	
	return CKR_OK;
}
