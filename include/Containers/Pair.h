/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_Pair.h
 @brief Pair Class
 @author minseob (rasidin1@hotmail.com)
 -----------------------------------------------------------
 History:
 - 2012/6/17 Created by minseob
 ***********************************************************/

#ifndef _LE_PAIR_H_
#define _LE_PAIR_H_

namespace LimitEngine {
    template<typename K, typename V> class Pair
    {
    public:
        Pair() {}
        Pair(const K& k, const V& v)
        {
            key = k;
            value = v;
        }
        Pair(const Pair &v)
        {
            key = v.key;
            value = v.value;
        }
		~Pair()
		{
			key.~K();
			value.~V();
		}
        K key;
        V value;
    };
}

#endif
