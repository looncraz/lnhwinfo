#include "Globals.h"
#include "Message.h"


void UPD(const char* str, const char* status)
{
	printf("\r\33[37m%s\33[36m%s                                              ", str, status);
	fflush(stdout);
}


void TestMessages()
{
	const char* base = "Testing HWMessage...";
	UPD(base, "");

	HWMessage	one("one"), two("two");
	SString		flat1, flat2;

	#define ADDSET(N, T, V1, V2) {								\
		UPD(base, "Add " #T "(" #V1 ")");						\
		one.Add (N, V1); 										\
		T out;													\
		if (!one.Find(N, out) || out != V1) {					\
			fprintf(stderr, "\n\33[31mTest failed! Add/Find\33[0m\n");		\
			return;	}											\
		UPD(base, "Set " #T "(" #V2 ")");						\
		one.Set (N, V2);										\
		if (!one.Find(N, out) || out != V2) {					\
			fprintf(stderr, "\n\33[31mTest failed! Set/Find\33[0m\n");		\
			return;	}											\
	}

	ADDSET("boolName", bool, true, false)
	ADDSET("unsignedName", uint64, 76543ULL, 33922133ULL)
	ADDSET("signedName", int64, -76543LL, -33922133LL)
	ADDSET("floatName", double, -3.14, 1.72)
	ADDSET("stringName", SString, SString("First"), "Second")

	// We don't support exporting to char* (Find(name, char**))
	// So we use the trick above to use both string literals and STL.

	SString sbuf = "DATA START:";

	for (int i = 0; i < 25; ++i) {
		if (sbuf.size() > 65536)
			sbuf = sbuf.substr(sbuf.size() - 65536, 65536);

		sbuf += sbuf + std::to_string(i);
		ADDSET(sbuf, bool, true, false)
		ADDSET(sbuf + "1", uint64, 76543ULL, 33922133ULL)
		ADDSET(sbuf + "2", int64, -76543LL, -33922133LL)
		ADDSET(sbuf + "3", double, -3.14, 1.72)
		ADDSET(sbuf + "4", SString, sbuf, sbuf + "4")
	}


	// data requires special handling...
	const char* data = "Data for me!";
	one.Add("dataName", data, strlen(data));

	char* buf = nullptr;
	int len = 0;
	if (!one.Find("dataName", (void**)&buf, &len)) {
		fprintf(stderr, "\n\33[31mData Add/Find failure!\33[0m\n");
		return;
	}

	two = one;
	UPD(base, "assign and compare");
	if (two != one) {
		fprintf(stderr, "\n\33[31mAssignment/comparison failure!\33[0m\n");
		return;
	}
	UPD(base, "move and compare");
	HWMessage three(std::move(two));

	three.Set("stringName", "bleh");

	if (three == one) {
		fprintf(stderr, "\n\33[31mFailure to mismatch after Set!\33[0m\n");
		return;
	}
	three.What = "three";

	if (three == one) {
		fprintf(stderr, "\n\33[31mFailure to mismatch after 'What'\33[0m\n");
		return;
	}

	two = one;
	two.What = "two";
	three.MakeEmpty();

	UPD(base, "flatten and unflatten");

	flat1 = one.Flatten();
	flat2 = two.Flatten();

	three.Unflatten(flat1);

	if (three != one) {
		fprintf(stderr, "\n\33[31mUnflattened message does not match original!\n");
		one.PrintToStream();
		three.PrintToStream();
		puts("\33[0m");
		return;
	}

	for (int i = 0; i < 100; ++i) {
		if (sbuf.size() > 65536)
			sbuf = sbuf.substr(sbuf.size() - 65536, 65536);

		sbuf += sbuf + std::to_string(i);
		ADDSET(sbuf, bool, true, false)
		ADDSET(sbuf + "1", uint64, 76543ULL, 33922133ULL)
		ADDSET(sbuf + "2", int64, -76543LL, -33922133LL)
		ADDSET(sbuf + "3", double, -3.14, 1.72)
		ADDSET(sbuf + "4", SString, sbuf, sbuf + "4")
	}

	UPD(base, "flatten heavy");
	flat1 = one.Flatten();
	UPD(base, "copy heavy");
	two = one;
	UPD(base, "empty");
	three.MakeEmpty();
	UPD(base, "unflatten heavy");
	three.Unflatten(flat1);
	UPD(base, "compare heavies");
	if (one != three || two != three) {
		fprintf(stderr, "FUCK!\n");
	}

	UPD(base, "empty one");
	one.MakeEmpty();

	UPD(base, "empty two");
	two.MakeEmpty();

	UPD(base, "empty three");
	three.MakeEmpty();

	#undef ADDSET
	UPD(base, "\33[32mDone");
	puts("\33[0m");
}

int main()
{
	TestMessages();
	puts("\33[0mEnd testing.");
	return 0;
}
