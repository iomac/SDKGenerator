#ifdef ENABLE_PLAYFABSERVER_API

#include "CppUnitTest.h"
#include "playfab/PlayFabServerDataModels.h"
#include "playfab/PlayFabServerApi.h"
#include "playfab/PlayFabSettings.h"
#include "Windows.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace PlayFab;
using namespace ServerModels;

namespace UnittestRunner
{
    TEST_CLASS(PlayFabServerTest)
    {
        static bool TITLE_INFO_SET;
        static string TEST_TITLE_DATA_LOC;
        static string testMessageReturn;

    public:
        /// <summary>
        /// PlayFab Title cannot be created from SDK tests, so you must provide your titleId to run unit tests.
        /// (Also, we don't want lots of excess unused titles)
        /// </summary>
        static void SetTitleInfo(web::json::value titleData)
        {
            TITLE_INFO_SET = true;

            // Parse all the inputs
            PlayFabSettings::titleId = titleData[U("titleId")].as_string();
            PlayFabSettings::developerSecretKey = titleData[U("developerSecretKey")].as_string();

            // Verify all the inputs won't cause crashes in the tests
            TITLE_INFO_SET = true;
        }

        TEST_CLASS_INITIALIZE(ClassInitialize)
        {
            if (TITLE_INFO_SET)
                return;

            // Prefer to load path from environment variable, if present
            char* envPath = getenv("PF_TEST_TITLE_DATA_JSON");
            if (envPath != nullptr)
                TEST_TITLE_DATA_LOC = envPath;

            ifstream titleInput;
            titleInput.open(TEST_TITLE_DATA_LOC, ios::binary | ios::in);
            if (titleInput)
            {
                auto begin = titleInput.tellg();
                titleInput.seekg(0, ios::end);
                auto end = titleInput.tellg();
                int size = static_cast<int>(end - begin);
                char* titleJson = new char[size + 1];
                titleInput.seekg(0, ios::beg);
                titleInput.read(titleJson, size);
                titleJson[size] = '\0';

                auto titleData = web::json::value::parse(WidenString(titleJson));
                SetTitleInfo(titleData);

                delete[] titleJson;
            }
        }
        TEST_CLASS_CLEANUP(ClassCleanup)
        {
        }

        static void PlayFabApiWait()
        {
            testMessageReturn = "pending";
            size_t count = 1, sleepCount = 0;
            while (count != 0)
            {
                count = PlayFabServerAPI::Update();
                sleepCount++;
                Sleep(1);
            }
            // Assert::IsTrue(sleepCount < 20); // The API call shouldn't take too long
        }

        // A shared failure function for all calls (That don't expect failure)
        static void SharedFailedCallback(const PlayFabError& error, void* customData)
        {
            testMessageReturn.clear();
            testMessageReturn._Grow(1024);
            testMessageReturn = "API_Call_Failed for: ";
            testMessageReturn += error.UrlPath;
            testMessageReturn += "\n";
            testMessageReturn += error.GenerateReport();
            testMessageReturn += "\n";
            testMessageReturn += ShortenString(error.Request.to_string());
        }

        /// <summary>
        /// SERVER API
        /// Test that CloudScript can be properly set up and invoked
        /// </summary>
        TEST_METHOD(ServerCloudScript)
        {
            ExecuteCloudScriptServerRequest hwRequest;
            hwRequest.FunctionName = "helloWorld";
            string playFabId = hwRequest.PlayFabId = "1337D00D";
            PlayFabServerAPI::ExecuteCloudScript(hwRequest, &CloudHelloWorldCallback, &SharedFailedCallback, nullptr);
            PlayFabApiWait();

            bool success = (testMessageReturn.find("Hello " + playFabId + "!") != -1);
            Assert::IsTrue(success, WidenString(testMessageReturn).c_str());
        }
        static void CloudHelloWorldCallback(const ExecuteCloudScriptResult& constResult, void* customData)
        {
            ExecuteCloudScriptResult result = constResult; // Some web::json::value syntax is unavailable for const objects, and there's just no way around it
            if (result.FunctionResult.is_null())
                testMessageReturn = "Cloud Decode Failure";
            else if (!result.Error.isNull())
                testMessageReturn = result.Error.mValue.Message;
            else
                testMessageReturn = ShortenString(result.FunctionResult[U("messageValue")].as_string());
        }
    };

    bool PlayFabServerTest::TITLE_INFO_SET = false;

    string PlayFabServerTest::TEST_TITLE_DATA_LOC = "C:/depot/pf-main/tools/SDKBuildScripts/testTitleData.json"; // TODO: Convert hard coded path to a relative path that always works (harder than it sounds when the unittests are run from multiple working directories)

    // Variables for specific tests
    string PlayFabServerTest::testMessageReturn;
}

#endif
