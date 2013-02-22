#pragma once

#pragma warning( push )
#pragma warning( disable : 4995 )

#include <vector>

template <typename T>
class Dictionary
{
public:
	Dictionary(int nSize = 1000) : m_hashTable(nSize, -1)
	{
	}

	void Add(const std::string& key, const T& data)
	{
		int hash = ComputeHash(key);

		int loopBreak = 0;
		for(;;)
		{
			if( m_hashTable[hash] == -1 )
			{
				m_hashTable[hash] = (int)m_data.size();
				m_data.push_back( std::make_pair(key, data) );
				break;
			}
			hash = (hash+1)%m_hashTable.size();
			++loopBreak;

			if( loopBreak > (int)m_hashTable.size() )
				break;
		}
	}

	T Find(const std::string& key, int* nDataIndex)
	{
		std::pair<int, T> pair = Find(key);

		if( nDataIndex )
			*nDataIndex = pair.first;
		return pair.second;
	}

	std::pair<int, T> Find(const std::string& key)
	{
		int hash = ComputeHash(key);

		int loopBreak = 0;
		for(;;)
		{
			if( m_hashTable[hash] == -1 )
				break;
			if( m_data[m_hashTable[hash]].first == key )
			{
				return std::make_pair(m_hashTable[hash], m_data[m_hashTable[hash]].second);
			}

			hash = (hash+1)%m_hashTable.size();
			++loopBreak;

			if( loopBreak > (int)m_hashTable.size() )
				break;
		}

		return std::make_pair(-1, (T)0);
	}

	void RemoveAll(bool bDeleteData = false)
	{
		if( bDeleteData )
		{
			for( auto i = m_data.begin(); i != m_data.end(); ++i )
			{
				if ( i->second ) delete i->second, i->second = 0;
			}
		}

		m_hashTable.assign(m_hashTable.size(), -1);
		m_data.resize(0);
	}

	std::vector<std::pair<std::string,T>>& GetDataArray()	{ return m_data; }

	inline unsigned int GetCount() const			{ return m_data.size(); }
	inline std::string GetKeyAt(int nIndex)	const 	{ return m_data[nIndex].first; }
	inline T GetValueAt(unsigned int nIndex) const	{ return m_data[nIndex].second; } 


protected:
	int ComputeHash(const std::string& key)
	{
		int hash = 0;

		for( int i = 0; i < (int)key.length(); ++i )
		{
			hash = (hash + key[0])%m_hashTable.size();
		}
		return hash;
	}


	std::vector<int> m_hashTable;
	std::vector<std::pair<std::string,T>> m_data;
};

#pragma warning ( pop )