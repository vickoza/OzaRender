#include "stdafx.h"
#include <deque>
#include <vector>
#include <algorithm>
#include <string>
#include "CppUnitTest.h"
#include <removeTrailingNotUnique.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace OzaRenderTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		

		TEST_METHOD(TestRemoveTrailingNotUniqueVectorIntNonRepeating1)
		{
			// TODO: Your test code here
			std::vector<int> testVector = { 1,5,7,3,9,12 };
			std::vector<int> checkVector = { 1,5,7,3,9,12 };
			Assert::IsTrue(testVector.end() == std::remove_trailing_not_unique(testVector.begin(), testVector.end()), L"Unexcepted return value");
			Assert::IsTrue(std::equal(testVector.begin(), testVector.end(), checkVector.begin(), checkVector.end()), L"tests did not match testVector with result vector");
		}

		TEST_METHOD(TestRemoveTrailingNotUniqueVectorIntRepeating1)
		{
			// TODO: Your test code here
			std::vector<int> testVector = { 1,5,1,7,5,1,7,3,9,3,12 };
			std::vector<int> checkVector = { 1,5,7,3,9,12 };
			testVector.erase(std::remove_trailing_not_unique(testVector.begin(), testVector.end()), testVector.end());
			Assert::IsTrue(std::equal(testVector.begin(), testVector.end(), checkVector.begin(), checkVector.end()), L"tests did not match testVector with result vector");
		}
	};
}