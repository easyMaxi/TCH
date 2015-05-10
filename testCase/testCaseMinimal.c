const char *testCaseMinimalDependencies[] = {"C_Initialize", "C_GetInfo", "C_Finalize", NULL};

int
testCaseMinimal(){
	CK_RV rv;
	CK_INFO Info;
	
	printf("Testing case: Minimal ...\n");
	printf(" Check dependencies ...\n");
	if(checkDependecies(testCaseMinimalDependencies) != 1) return CKR_FUNCTION_NOT_SUPPORTED;
	printf(" Dependencies are fine\n");
	
	//testing finalize before initialize
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_CRYPTOKI_NOT_INITIALIZED) printf(" unexpected intialized cryptoki before initalize\n");
	
	//initialize
	printf(" call Initialize ...");
	rv = pFunctionList->C_Initialize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv, 0);
	else printf(" success\n");
	
	//getinfo
	printf(" call GetInfo ...");
	rv = pFunctionList->C_GetInfo(&Info);
	if(rv != CKR_OK) returnTestCase(rv,0);
	else printf(" success\n");
	
	//finalize
	printf(" call Finalize ...");
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_OK) returnTestCase(rv,0);
	else printf(" success\n");
	
	//try finalize a second time
	rv = pFunctionList->C_Finalize(NULL_PTR);
	if(rv != CKR_CRYPTOKI_NOT_INITIALIZED) printf(" unexpected intialized cryptoki after finalize\n");
	
	printf(" ... case successful\n");
	
	return CKR_OK;
}
