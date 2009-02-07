#pragma once

enum
{
	GIT_SUCCESS=0,
	GIT_ERROR_OPEN_PIP,
	GIT_ERROR_CREATE_PROCESS,
	GIT_ERROR_GET_EXIT_CODE
};

extern BOOL g_IsWingitDllload;

class CGitByteArray:public std::vector<BYTE>
{
public:
	int find(BYTE data,int start=0)
	{
		for(int i=start;i<size();i++)
			if( at(i) == data )
				return i;
		return -1;
	}
	int findNextString(int start=0)
	{
		int pos=start;
		do
		{
			pos=find(0,pos);
			if(pos >= 0)
				pos++;
			else
				break;

			if( pos >= size())
				return -1;

		}while(at(pos)==0);

		return pos;
	}
	int findData(const BYTE* dataToFind, size_t dataSize, int start=0)
	{
		const BYTE* pos=&*begin()+start;
		const BYTE* dataEnd=&*end()-(dataSize-1);//Set end one step after last place to search
		if(pos>=dataEnd)
			return -1;//Started over end. Return not found
		if(dataSize==0)
			return start;//No search data. Return current position
		BYTE firstByte=dataToFind[0];
		while(pos<dataEnd)
		{
			//memchr for first character
			const BYTE* found=(const BYTE*)memchr(pos,firstByte,dataEnd-pos);
			if(found==NULL)
				return -1;//Not found
			//check rest of characters
			if(memcmp(found,dataToFind,dataSize)==0)
				return found-&*begin();//Match. Return position.
			//No match. Set position on next byte and continue search
			pos=found+1;
		}
		return -1;
	}
	int append( std::vector<BYTE> &v,int start=0,int end=-1)
	{
		if(end<0)
			end=v.size();
		for(int i=start;i<end;i++)
			this->push_back(v[i]);
		return 0;
	}
	int append(const BYTE* data, size_t dataSize)
	{
		size_t oldsize=size();
		resize(oldsize+dataSize);
		memcpy(&*(begin()+oldsize),data,dataSize);
		return 0;
	}
};
typedef std::vector<CString> STRING_VECTOR;
typedef std::map<CString, STRING_VECTOR> MAP_HASH_NAME;
typedef CGitByteArray BYTE_VECTOR;