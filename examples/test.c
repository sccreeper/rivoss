const char *testString = "Hello World!";
#define testStringLength 12
#define iterations 100000

char stupidlyLongArray[iterations];

int main() {

    int x = 4;
    int y = 3;

    int testStringOffset = 0;

    for (int i = 0; i < iterations; i++) {
        x = x * y;
        y += 3;

        if (testStringOffset == testStringLength + 1) {
            testStringOffset = 0;
        }

        y += testString[testStringOffset];
        stupidlyLongArray[i] = testStringOffset;

        testStringOffset++;
    }

    return 0;
}
