#include "Globals.h"
#include "Message.h"
#include <iostream>

#define kBoolType 	"BOOLEAN"
#define kIntType 	"INTEGER"
#define kFloatType 	"FLOAT"
#define kStringType	"STRING"
#define kDataType	"DATA"


class HWMessage::HWMData {
public:
	HWMData	(){
		puts("new HWMData");
	}
	~HWMData(){
		puts("del  HWMData");
	}

	HWMData(CString& unflatten)
	{
		printf("HWMData (%s)\n", unflatten.c_str());

		auto div = unflatten.find(":");
		if (div == SString::npos) return;

		type = unflatten.substr(0, div);

		// yeah, just checking the first byte... sue me!
		switch (type[0]) {
			case 'B':case 'I':case 'F':case 'S':case 'D':break;
			default:
			type = "";
			return;
		}	// TODO: maybe convert type to a char?
		div += 1;
		value = unflatten.substr(div, unflatten.size() - div);
	}

	// type, entries
	SString					type;
	SString					value;

	#define SET_TO(X, Y) void SetTo(X val) { type = Y; value = std::to_string(val);	\
		printf("%s:%s\n", type.c_str(), value.c_str()); }

	SET_TO(bool,	kBoolType)
	SET_TO(uint64,	kIntType)
	SET_TO(int64, 	kIntType)
	SET_TO(double,	kFloatType)

	#undef SET_TO

	void SetTo(CString& val)
	{
		type = kStringType;
		value = val;
	}

	void SetTo(const void* _data, int len)
	{
		type = kDataType;
		value = "";

		const char* data = (const char*)_data;
		for (int i = 0; i < len; ++i)
			value += data[i];
	}


	SString ToString() const
	{
		return type + SString(":") + value;
	}

	bool Get(bool& out) const
	{
		if (type != kBoolType) return false;
		out = value == std::to_string(true);
		return true;
	}

	bool Get(uint64& out) const
	{
		if (type != kIntType) return false;
		out = (uint64)std::stoll(value);
		return true;
	}

	bool Get(int64& out) const
	{
		if (type != kIntType) return false;
		out = (int64)std::stoll(value);
		return true;
	}

	bool Get(double& out) const
	{
		if (type != kFloatType) return false;
		out = atof(value.c_str());
		return true;
	}

	bool Get(SString& out) const
	{
		if (type != kStringType) return false;
		out = value;
		return true;
	}

	bool Get(void** out, int* len) const
	{
		if (type != kDataType) return false;
		*len = 0;
		*out = malloc(value.size());
		if (*out == nullptr) {
			fprintf(stderr, "malloc failure!\n");
			return false;
		}
		*len = value.size();
		return true;
	}

};



HWMessage::HWMessage(CString& what)
	:
	What(what)
{
}


HWMessage::~HWMessage	()
{
	MakeEmpty();
}


void
HWMessage::PrintToStream	() const
{
	std::cout << "HWMessage = " << What << " {" << std::endl;

	bool trunc = false;

	for (auto& [key, data] : fData) {
		printf("\t%-20.20s%s = (%8s){%30.30s%s}\n", key.c_str(),
			key.size() > 20 ? "..." : "   ",
			data->type.c_str(),
 			data->value.c_str(),
			data->value.size() > 30 ? "..." : "   "
			);

		if (data->type.size() > 20 || data->value.size() > 30)
			trunc = true;
	}
	if (trunc)
		std::cout << "\t(Some names / values were truncated to fit)\n";

	std::cout << "}\n";
}


bool
HWMessage::HasName(CString& name) const
{
	return fData.find(name) != fData.end();
}


void
HWMessage::MakeEmpty()
{
	What = "";
	for (auto& [key, data] : fData)
		delete data;

	fData.clear();
}


void
HWMessage::Remove(CString& name)
{
	auto iter = fData.find(name);
	if (iter == fData.end())
		return;

	delete fData[name];
	fData.erase(iter);
}


#define DATA_FLAT_SEP "{un1Qu3}"


SString
HWMessage::Flatten() const
{
	SString flat = "HWMessage{";
	flat += What.size();
	flat += ":" + What + "}{";

	for (auto& [key, data] : fData) {
		flat += key + "={" + data->ToString() + "}" + DATA_FLAT_SEP;
	}

	flat += "}";
	return flat;
}


bool
HWMessage::Unflatten(CString& input)
{
	MakeEmpty();
	try {
	if (input.find("HWMessage{") != 0)
		return false;

	auto start = 10;
	auto end = input.find(":", start);
	if (end == SString::npos)
		return false;

	SString buf = input.substr(start, end - start);

	if (buf.size() > 10) {
		fprintf(stderr, "Unflatten: %s is an invalid What length!\n",
			buf.c_str());
		return false;
	}

	int64 len = std::stoll(buf);
	start = end + buf.size();
	end = input.find("}{", start);
	if (end == SString::npos) {
		fprintf(stderr, "Unflatten: can't find data start!\n");
		return false;
	}

	What = input.substr(start, end - start);

	// grab entire data contents - minus one for trailin '}'
	start += len + 2;
	buf = input.substr(start, buf.size() - start);
	buf.pop_back(); // get rid of '}'
	auto entries = HWUtils::SplitString(buf, DATA_FLAT_SEP);

	if (entries.size() == 0 && buf.size() > 30)	{
		fprintf(stderr, "Unflatten: unable to parse!\n");
		MakeEmpty();
		return false;
	}

	// line == name={type:value}
	for (auto& line : entries) {
		printf("IN: \"%s\"\n", line.c_str());
		start = line.find("=");
		if (start == SString::npos) {
			fprintf(stderr, "Unflatten: can't parse \"%s\"\n", line.c_str());
			continue;
		}

		end = line.size();
		fData[line.substr(0, start)]
 			= new HWMData(line.substr(start + 1, end - start));
	}
	} catch (std::exception& e) {
		fprintf(stderr, "Unflatten: Caught exception: %s!\n", e.what());
		return false;
	}
	return true;
}

#define FIND_FUNC(N, T) bool HWMessage::Find##N (CString& name, T & out) const	\
{	if (!HasName(name))	return false; return fData.at(name)->Get(out); }


FIND_FUNC(Bool, bool)
FIND_FUNC(Integer, uint64)
FIND_FUNC(Integer, int64)
FIND_FUNC(Float, double)
FIND_FUNC(String, SString)


bool
HWMessage::FindData	(CString& name, void** out, int* len) const
{
 if (!HasName(name))
   return false;

 return fData.at(name)->Get(out, len);
}


#define ADD_FUNC(N, T) bool HWMessage::Add##N (CString& name, T value)	\
{	if (HasName(name))	return false; \
	return Set##N (name, value); }

ADD_FUNC(Bool, bool)
ADD_FUNC(Integer, uint64)
ADD_FUNC(Integer, int64)
ADD_FUNC(Float, double)
ADD_FUNC(String, CString&)


bool
HWMessage::AddData		(CString& name, const void* data, int len)
{
 if (HasName(name))
   return false;

 return SetData(name, data, len);
}

#define SET_FUNC(N, T) bool HWMessage::Set##N(CString& name, T value)	\
{	auto& data = fData[name];							\
	if (data == nullptr) data = new HWMData();			\
	data->SetTo(value);									\
	return true;										\
}


SET_FUNC(Bool, bool)
SET_FUNC(Integer, uint64)
SET_FUNC(Integer, int64)
SET_FUNC(Float, double)
SET_FUNC(String, CString&)


bool
HWMessage::SetData(CString& name, const void* ptr, int len)
{
	if (len == 0 || ptr == nullptr)
		return false;

	auto* data = fData[name];
	if (data == nullptr) data = new HWMData();
	data->SetTo(ptr, len);
	return true;
}


void
HWMessage::ForEach(std::function<void(CString&, CString&)>& func) const
{
	for (auto& [name, data] : fData) {
		if (data == nullptr) {
			fprintf(stderr, "HWMessage: BUG!  nullptr in data!\n");
			continue;
		}

		func(name, data->ToString());
	}
}
