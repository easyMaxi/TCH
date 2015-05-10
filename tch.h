#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h> //dlsym
#include <unistd.h> //getopt
#include <time.h>
#include <sys/time.h>
#include "pkcs11_linux.h"

struct arguments
{
	char *filename;
	char *module;
	int mode;
	int initiate;
	int verbose;
	char *uPin;
	char *soPin;
};
struct arguments sArguments = 
{
	.mode = 0,
	.initiate = 0,
	.verbose = 0,
	.filename = NULL,
	.module = NULL,
	.uPin = NULL,
	.soPin = NULL,
};

CK_FUNCTION_LIST_PTR pFunctionList = NULL;

struct mechaInfo
{
    CK_MECHANISM_INFO info;
    CK_MECHANISM_TYPE type;
};
struct mechaInfo *pMechaInfoList = NULL;
int pMechaInfoListLen = 0;

#include "pkcs11OutputConstants.c"
#include "testCase/testCaseHelper.c"

#include "test_functions.c"
#include "print_information.c"


//test case
#include "testCase/testCaseMinimal.c" //1
#include "testCase/testCaseSlotInformation.c" //2
#include "testCase/testCaseTokenInformation.c" //3
#include "testCase/testCaseCompareMechanismLists.c" //4
#include "testCase/testCaseSlotEvents.c" //5
#include "testCase/testCaseSingleSessionWithUserLogin.c" //6
#include "testCase/testCaseMultipleSessionWithUserLogin.c" //7
#include "testCase/testCasePreserveOperationState.c" // 8
#include "testCase/testCaseAdminToken.c" //9
#include "testCase/testCaseChangePin.c" //10
#include "testCase/testCaseObjectHandling.c" //11
#include "testCase/testCaseObjectSearch.c" //12
#include "testCase/testCaseDeAndEncryptionSimple.c" //13
#include "testCase/testCaseDeAndEncryptionMultiple.c" //14
#include "testCase/testCaseDigestSimple.c" //15
#include "testCase/testCaseDigistMultiple.c" //16
#include "testCase/testCaseSignAndVerifySimple.c" //17
#include "testCase/testCaseSignAndVerifyMultiple.c" //18
#include "testCase/testCaseSignAndVerifyRecover.c" //19
#include "testCase/testCaseDualPurposeFunctions.c" //20
#include "testCase/testCaseSingleKeyGeneration.c" //21
#include "testCase/testCaseKeyPairGeneration.c" //22
#include "testCase/testCaseWrapAndUnwrapKey.c" //23
#include "testCase/testCaseDeriveKey.c" //24
#include "testCase/testCaseGenerateRandom.c" //25

