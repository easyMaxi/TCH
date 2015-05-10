#include "tch.h"


int check_arguments(int argc, char **argv, CK_FUNCTION_LIST_PTR_PTR pFunctionList)
{
	int opt;
	CK_RV rv;
	CK_C_GetFunctionList tch_C_GetFunctionList;
	
	while ((opt = getopt(argc, argv, "-o-i-a-m:-u:-s:-v")) != -1)
	{
		switch (opt)
		{
			case 'o':
				sArguments.mode = 1;
				break;
			case 'm':
				sArguments.filename = optarg;
				break;
			case 'i':
				sArguments.initiate = 1;
				break;
			case 's':
				sArguments.soPin = optarg;
				break;
			case 'u':
				sArguments.uPin = optarg;
				break;
			case 'v':
				sArguments.verbose = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-o] [-i] [-v] [-m file] [-u User PIN] [-s SO PIN]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	
	if(sArguments.filename) //try loading the module
	{
		sArguments.module = dlopen(sArguments.filename, RTLD_NOW);
		if (!sArguments.module) printf("Couldn't open library: %s: %s\n", sArguments.filename, dlerror ());

		//try to find what function we can use
		tch_C_GetFunctionList = (CK_C_GetFunctionList) dlsym (sArguments.module, "C_GetFunctionList");
		if (!tch_C_GetFunctionList)
		{
			printf("Couldn't find function 'C_GetFunctionList' in library: %s: %s\n", sArguments.filename, dlerror ());
		}
		else
		{
			rv = tch_C_GetFunctionList(pFunctionList);
			if(rv != CKR_OK) returnValuePrinting("C_GetFunctionList", rv);
			else
			{

			}
			
		}
	}	
	return 0;
}

int
main(int argc, char **argv)
{
	check_arguments(argc, argv, &pFunctionList);

	if(pFunctionList)
	{
		testFunctionsImplemented();
		printTokenInformation();
	
		testCaseMinimal();
		testCaseSlotInformation();
		testCaseTokenInformation();

		testCaseCompareMechanismLists();
		testCaseSlotEvents();

		testCaseSingleSessionWithUserLogin();
		testCaseMultipleSessionWithUserLogin();
		testCasePreserveOperationState();
		
		testCaseAdminToken();
		testCaseChangePin();

		testCaseObjectHandling();
		testCaseObjectSearch();

		testCaseDeAndEncryptionSimple();
		testCaseDeAndEncryptionMultiple();

		testCaseDigestSimple();
		testCaseDigestMultiple();
		
		testCaseSignAndVerifySimple();
		testCaseSignAndVerifyMultiple();
		
		testCaseSignAndVerifyRecover();
		testCaseDualPurposeFunctions();
		
		testCaseSingleKeyGeneration();
		testCaseKeyPairGeneration();

		testCaseWrapAndUnwrapKey();
		testCaseDeriveKey();

		testCaseGenerateRandom();
	}
	
	free(pMechaInfoList);
	return 0;
}
