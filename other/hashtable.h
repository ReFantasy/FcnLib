#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__
#include <vector>
#include <list>
#define __NamespaceTdlBegin namespace TDL {
#define __NamespaceTdlEnd }

__NamespaceTdlBegin

using std::vector;
using std::list;

// 默认的散列函数对象
template<typename Ty>
struct HashFcnDefault
{
	size_t operator()(const Ty &obj)
	{
		int obj_sz_by_byte = sizeof(obj);
		const unsigned char*p = reinterpret_cast<const unsigned char*>(&obj);

		int hash_code = 0;
		int weight = 1;
		for (int i = 0; i < obj_sz_by_byte; i++)
		{
			hash_code += (*p)*weight;
			weight *= 256;
			p++;
		}

		return hash_code;
	}
};

// 定义散列表
template<typename ValueTy, typename Key = ValueTy, typename ExtractKey = ::std::identity<ValueTy>, typename HashFcn = HashFcnDefault<Key>, int BucketCount = 53>
class HashTable
{
public:
	using value_type = ValueTy;
	using List = list<value_type>;
	using hash = HashFcn;
public:
	HashTable() :buckets(BucketCount, List()) {}
	bool insert(value_type &insert_value);
	bool remove(const Key &key);
	ValueTy* find(Key key)const;
	size_t GetBucketSize()const;
	size_t GetElementSize()const;


private:
	bool ExtendBucket();
private:
	vector<List> buckets;
	size_t element_count = 0;
	int bucket_count_index = 0;
};

template<typename ValueTy, typename Key /*= ValueTy*/, typename ExtractKey /*= identity<ValueTy>*/, typename HashFcn /*= HashFcnDefault<Key>*/, int BucketCount /*= 53*/>
bool HashTable<ValueTy, Key, ExtractKey, HashFcn, BucketCount>::remove(const Key &key)
{
	size_t index = hash()(key) % prime_list[bucket_count_index];
	List &tmp_list = buckets[index];

	for (auto it = tmp_list.begin(); it != tmp_list.end(); it++)
	{
		if (ExtractKey()(*it) == key)
		{
			tmp_list.erase(it);
			return true;
		}
	}
	return false;
}

template<typename ValueTy, typename Key, typename ExtractKey, typename HashFcn, int BucketCount >
size_t HashTable<ValueTy, Key, ExtractKey, HashFcn, BucketCount>::GetElementSize() const
{
	return element_count;
}

template<typename ValueTy, typename Key, typename ExtractKey, typename HashFcn, int BucketCount >
size_t HashTable<ValueTy, Key, ExtractKey, HashFcn, BucketCount>::GetBucketSize() const
{
	return buckets.size();
}

static const int num_primes = 28;
static const size_t prime_list[num_primes] =
{
	53,         97,         193,       389,       769,
	1543,       3079,       6151,      12289,     24593,
	49157,      98317,      196613,    393241,    786433,
	1572869,    3145739,    6291469,   12582917,  25165843,
	50331653,   100663319,  201326611, 402653189, 805306457,
	1610612741, 3221225473, 4294967291
};


template<typename ValueTy, typename Key, typename ExtractKey, typename HashFcn, int BucketCount >
ValueTy* HashTable<ValueTy, Key, ExtractKey, HashFcn, BucketCount>::find(Key key) const
{
	size_t index = hash()(key) % prime_list[bucket_count_index];;
	for (auto &e : buckets[index])
	{
		if (ExtractKey()(e) == key)
		{
			return const_cast<ValueTy*>(&e);
		}

	}
	return nullptr;
}


template<typename ValueTy, typename Key, typename ExtractKey, typename HashFcn, int BucketCount>
bool HashTable<ValueTy, Key, ExtractKey, HashFcn, BucketCount>::insert(value_type &insert_value)
{
	size_t index = hash()(ExtractKey()(insert_value)) % prime_list[bucket_count_index];
	if (buckets[index].size() >= prime_list[bucket_count_index])
	{// 判断是否需要重新散列
		bool rehash = ExtendBucket();
		if (!rehash) // 扩容失败
			return false;
	}

	// 插入新的元素
	index = hash()(ExtractKey()(insert_value)) % prime_list[bucket_count_index];
	buckets[index].push_back(std::move(insert_value));
	element_count++;
	return true;
}


template<typename ValueTy, typename Key, typename ExtractKey, typename HashFcn, int BucketCount>
bool HashTable<ValueTy, Key, ExtractKey, HashFcn, BucketCount>::ExtendBucket()
{
	//先判断是否还有扩展空间
	if (bucket_count_index + 1 >= num_primes)
		return false;

	// 扩展Bucket;
	bucket_count_index++;
	vector<List> new_buckets(prime_list[bucket_count_index], List());
	// 将旧数据移动到new_buckets
	for (auto &l : buckets)
	{
		if (l.size() > 0)
		{
			for (auto &e : l)
			{
				size_t index = hash()(ExtractKey()(e)) % prime_list[bucket_count_index];
				new_buckets[index].push_back(std::move(e));
			}
		}
	}
	buckets = new_buckets;
	return true;
}

__NamespaceTdlEnd
#endif // __HASHTABLE_H__
