int
findObjects(CK_SLOT_ID slotID, CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR objectTemplate)
{
	
	CK_OBJECT_HANDLE hData;
	CK_ULONG ulObjectCount;
	CK_RV rv;
	int count = 0;

	
	rv = pFunctionList->C_FindObjectsInit(hSession, objectTemplate, 0);
	if(rv != CKR_OK)returnValuePrinting("C_FindObjectsInit", rv);
	while (1) 
	{   
		rv = pFunctionList->C_FindObjects(hSession, &hData, 1, &ulObjectCount);   
		if(rv != CKR_OK) returnValuePrinting("C_FindObjects", rv);
		if (rv != CKR_OK || ulObjectCount == 0) break;
		count++;
		if(sArguments.mode) pFunctionList->C_DestroyObject(hSession, hData);
	}
	rv = pFunctionList->C_FindObjectsFinal(hSession);
	if(rv != CKR_OK) returnValuePrinting("C_FindObjectsFinal", rv);
	
	printf(" Could find %d objects of class %lu\n", count, *((CK_ULONG *) objectTemplate->pValue));

	
	return 0;
}


int
printTokenInformation()
{
	CK_RV rv;
	CK_SLOT_ID slotID;
	CK_SLOT_ID_PTR pSlotList;
	CK_SLOT_INFO slotInfo;
	CK_TOKEN_INFO tokenInfo;
	CK_INFO info;
	CK_ULONG ulSlotCount;
	CK_MECHANISM_TYPE_PTR pMechanismList;
	CK_MECHANISM_INFO sMechaInfo;
	CK_ULONG mCount;
	CK_BYTE application = 42;
	CK_NOTIFY myNotify = NULL;
	CK_SESSION_HANDLE hSession;
	CK_OBJECT_CLASS dataClass = CKO_DATA;
	CK_OBJECT_CLASS certficateClass = CKO_CERTIFICATE;
	CK_OBJECT_CLASS privateKeyClass = CKO_PRIVATE_KEY;
	CK_OBJECT_CLASS otpKeyClass = CKO_OTP_KEY;
	CK_OBJECT_CLASS publicKeyClass = CKO_PUBLIC_KEY;
	CK_OBJECT_CLASS secretKeyClass = CKO_SECRET_KEY;
	CK_OBJECT_CLASS vendorDefinedClass = CKO_VENDOR_DEFINED;
	CK_ATTRIBUTE dataTemplate[] = { {CKA_CLASS, &dataClass, sizeof(dataClass)} };
	CK_ATTRIBUTE certificateTemplate[] = { {CKA_CLASS, &certficateClass, sizeof(CKO_CERTIFICATE)} };
	CK_ATTRIBUTE privateKeyTemplate[] = { {CKA_CLASS, &privateKeyClass, sizeof(privateKeyClass)} };
	CK_ATTRIBUTE otpKeyTemplate[] = { {CKA_CLASS, &otpKeyClass, sizeof(otpKeyClass)} };
	CK_ATTRIBUTE publicKeyTemplate[] = { {CKA_CLASS, &publicKeyClass, sizeof(publicKeyClass)} };
	CK_ATTRIBUTE secretKeyTemplate[] = { {CKA_CLASS, &secretKeyClass, sizeof(secretKeyClass)} };
	CK_ATTRIBUTE vendorDefinedTemplate[] = { {CKA_CLASS, &vendorDefinedClass, sizeof(vendorDefinedClass)} };
	int i;

	//C_Initialize
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnValuePrinting("C_Initialize", rv);
	
	//C_GetInfo
	rv = pFunctionList->C_GetInfo(&info);
	if(rv != CKR_OK) returnValuePrinting("C_GetInfo", rv);
	if(rv == CKR_OK)
	{
		printf("C_GetInfo ...\n");
		printf(" Cryptoki Version: %u.%u\n", info.cryptokiVersion.major, info.cryptokiVersion.minor);
		printf(" Manufacturer ID: %.*s\n", 32, info.manufacturerID);
		printf(" Flags: %lu (should be null)\n", info.flags);	
		printf(" Library Description: %.*s\n", 32, info.libraryDescription);
		printf(" Library Version: %u.%u\n", info.libraryVersion.major, info.libraryVersion.minor);
	}
	
	
	//C_GetSlotInfo & C_GetTokenInfo
	rv = pFunctionList->C_GetSlotList(CK_TRUE, NULL_PTR, &ulSlotCount); //we need the number of slots
	if(rv != CKR_OK) returnValuePrinting("C_GetSlotList", rv);
	
	pSlotList = malloc(ulSlotCount * sizeof(CK_SLOT_ID)); //make a list	
	
	rv = pFunctionList->C_GetSlotList(CK_TRUE, pSlotList, &ulSlotCount); //and fill the list
	if(rv != CKR_OK) returnValuePrinting("C_GetSlotList", rv);
	
	for(i=0;i<ulSlotCount;i++) //iterate over them and try GetSlotInfo()
	{
		slotID = pSlotList[i];
		rv = pFunctionList->C_GetSlotInfo(slotID, &slotInfo);
		if(rv != CKR_OK) returnValuePrinting("C_GetSlotInfo", rv);
		if(rv == CKR_OK)
		{
			printf("C_GetSlotInfo on slot %lu\n", slotID);
			printf(" Slot Description: %.*s\n", 64, slotInfo.slotDescription);
			printf(" Manufacturer ID: %.*s\n", 32, slotInfo.manufacturerID);
			printf(" Flags(%lu): ", slotInfo.flags);
			printFlags(slotInfo.flags, CK_SLOT_INFO_FLAG_LIST);
			printf(" Hardware Version: %u.%u\n", slotInfo.hardwareVersion.major, slotInfo.hardwareVersion.minor);
			printf(" Firmware Version: %u.%u\n", slotInfo.firmwareVersion.major, slotInfo.firmwareVersion.minor);
		}
		rv = pFunctionList->C_GetTokenInfo(slotID, &tokenInfo);
		if(rv != CKR_OK) returnValuePrinting("C_GetTokenInfo", rv);
		if(rv == CKR_OK)
		{
			printf("C_GetTokenInfo on slot %lu\n", slotID);
			printf(" Label: %.*s\n", 32, tokenInfo.label);
			printf(" Manufacturer ID: %.*s\n", 32, tokenInfo.manufacturerID);
			printf(" Model: %.*s\n", 16, tokenInfo.model);
			printf(" Serialnumber: %.*s\n", 16, tokenInfo.serialNumber);
			printf(" Flags (%lu): ", tokenInfo.flags);
			printFlags(tokenInfo.flags, CK_TOKEN_INFO_FLAG_LIST);
			printf(" Max sessions: %lu\n", tokenInfo.ulMaxSessionCount);
			printf(" Open sessions: %lu\n", tokenInfo.ulSessionCount);
			printf(" Max R/W sessions: %lu\n", tokenInfo.ulMaxRwSessionCount);
			printf(" Open R/W session: %lu\n", tokenInfo.ulRwSessionCount);
			printf(" Max. PIN length: %lu\n", tokenInfo.ulMaxPinLen);
			printf(" Min. PIN length: %lu\n", tokenInfo.ulMinPinLen);
			printf(" Public memory: %lu\n", tokenInfo.ulTotalPublicMemory);
			printf(" Free public memory: %lu\n", tokenInfo.ulFreePublicMemory);
			printf(" Private memory: %lu\n", tokenInfo.ulTotalPrivateMemory);
			printf(" Free private memory: %lu\n", tokenInfo.ulFreePrivateMemory);
			printf(" Hardware Version: %u.%u\n", tokenInfo.hardwareVersion.major, tokenInfo.hardwareVersion.minor);
			printf(" Firmware Version: %u.%u\n", tokenInfo.firmwareVersion.major, tokenInfo.firmwareVersion.minor);
			printf(" Time: %.*s\n", 16,tokenInfo.utcTime);
		}
	}
	
	
	//C_GetMechanismList
	for(i=0;i<ulSlotCount;i++)// for every slot try to get the mechanism list
	{
		slotID = pSlotList[i];
		rv = pFunctionList->C_GetMechanismList(slotID, NULL_PTR, &mCount);//how many ?
		if(rv != CKR_OK) returnValuePrinting("C_GetMechanismList", rv);	
		
		pMechanismList = (CK_MECHANISM_TYPE_PTR) calloc(mCount, sizeof(CK_MECHANISM_TYPE));
		
		rv = pFunctionList->C_GetMechanismList(slotID, pMechanismList, &mCount);//give me the mechanisms
		if(rv != CKR_OK) returnValuePrinting("C_GetMechanismList", rv);
		
		pMechaInfoListLen = mCount;
		pMechaInfoList = calloc(pMechaInfoListLen, sizeof(struct mechaInfo));
		
		printf("C_GetMechanismList on slot %lu\n", slotID);
		int j,k;
		for(j=0;j<mCount;j++)
		{
			pMechaInfoList[j].type = pMechanismList[j];//fill the global mechaInfoList with types 
			k = searchEleName(pMechanismList[j], MECHANISM_LIST, (sizeof(MECHANISM_LIST)/sizeof(*MECHANISM_LIST)));
			if(k != -1)
			{
				printf("%s, ", MECHANISM_LIST[k].eleName);
			}
			else
			{
				printf("%lu, ", pMechanismList[j]);
			}
		}
		free(pMechanismList);

		printf("\n");	
	}
	
	//C_GetMechanismInfo
	for(i=0;i<ulSlotCount;i++) //foreach slot search for mechanisms
	{
		slotID = pSlotList[i];
		int j = 0;		
		printf("C_GetMechanismInfo on slot %lu\n", slotID);
		while((MECHANISM_LIST[j].eleName != NULL)) //run through all known mechanisms
		{
			rv = pFunctionList->C_GetMechanismInfo(slotID, MECHANISM_LIST[j].eleValue, &sMechaInfo);
			if((rv != CKR_OK) & (rv != CKR_MECHANISM_INVALID)) returnValuePrinting("C_GetMechanismInfo", rv);
			if((rv != CKR_MECHANISM_INVALID) & (rv != CKR_TOKEN_NOT_PRESENT))//device supports the mecha
			{
				int k;
				for(k=0;k<pMechaInfoListLen;k++)
				{
					if(pMechaInfoList[k].type == MECHANISM_LIST[j].eleValue) pMechaInfoList[k].info = sMechaInfo;	
				}
				printf(" %s: key size(%lu-%lu)",
					MECHANISM_LIST[j].eleName, 
					sMechaInfo.ulMinKeySize, 
					sMechaInfo.ulMaxKeySize);
				printFlags(sMechaInfo.flags, CK_MECHANISM_INFO_FLAG_LIST);
			}
			j++;
		}
	}
	
	if(sArguments.uPin)
	{
		//Show User Objects
		for(i=0;i<ulSlotCount;i++) //foreach slot
		{
			printf("Find objects on slot %lu as User\n", slotID);
			slotID = pSlotList[i];
			
			
			//OpenSession with CKF_SERIAL_SESSION
			rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION | CKF_RW_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
			if(rv != CKR_OK) returnValuePrinting("C_OpenSession", rv);
			

			//Login
			rv = pFunctionList->C_Login(hSession, CKU_USER, (CK_UTF8CHAR_PTR) sArguments.uPin, strlen(sArguments.uPin));
			if(rv != CKR_OK) exit(EXIT_FAILURE);

			
			//find objects of each class
			findObjects(slotID, hSession, dataTemplate);
			findObjects(slotID, hSession, certificateTemplate);
			findObjects(slotID, hSession, privateKeyTemplate);
			findObjects(slotID, hSession, otpKeyTemplate);
			findObjects(slotID, hSession, publicKeyTemplate);
			findObjects(slotID, hSession, secretKeyTemplate);
			findObjects(slotID, hSession, vendorDefinedTemplate);
			

			//Logout
			rv = pFunctionList->C_Logout(hSession);
			if(rv != CKR_OK) returnValuePrinting("C_Logout", rv);

			//CloseSession
			rv = pFunctionList->C_CloseSession(hSession);
			if(rv != CKR_OK) returnValuePrinting("C_CloseSession", rv);
		}	
	}
	
	//Show Objects
	for(i=0;i<ulSlotCount;i++) //foreach slot
	{
		printf("Find public objects on slot %lu\n", slotID);
		slotID = pSlotList[i];
		
		
		//OpenSession with CKF_SERIAL_SESSION
		rv = pFunctionList->C_OpenSession(slotID, CKF_SERIAL_SESSION, (CK_VOID_PTR) &application, myNotify, &hSession);
		if(rv != CKR_OK) returnValuePrinting("C_OpenSession", rv);
		
	
		//find objects of each class
		findObjects(slotID, hSession, dataTemplate);
		findObjects(slotID, hSession, certificateTemplate);
		findObjects(slotID, hSession, privateKeyTemplate);
		findObjects(slotID, hSession, otpKeyTemplate);
		findObjects(slotID, hSession, publicKeyTemplate);
		findObjects(slotID, hSession, secretKeyTemplate);
		findObjects(slotID, hSession, vendorDefinedTemplate);
		

		//CloseSession
		rv = pFunctionList->C_CloseSession(hSession);
		if(rv != CKR_OK) returnValuePrinting("C_CloseSession", rv);
	}
	
	
	//free memory
	free(pSlotList);
	
	//C_Finalize
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) return 1;
	
	return 0;
}
