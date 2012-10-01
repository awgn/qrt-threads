/* $Id$ */
/* 
 * qrt::thread++ - LGPL library 
 *
 * Copyright (C) 2010 Nicola Bonelli
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _QRT_HEAP_HPP_
#define _QRT_HEAP_HPP_ 

#include <functional>
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <memory>
#include <map>

namespace qrt { 

    // SGI http://www.sgi.com/tech/stl/make_heap.html: 
    // A heap is a particular way of ordering the elements in a range of Random Access Iterators [f, l)
    // This heap implementation is based on the SGI algorithm make_heap/pop_heap/push_heap.

    namespace random_access {

        template <typename K, typename V, template <typename Ty, typename Alloc = std::allocator<Ty> > class _Cont>
        class base_heap
        {
        public:
            typedef K               key_type;
            typedef V               mapped_type;
            typedef std::pair<K,V>  value_type;

        private:
            _Cont<value_type>  _M_cont;

            // compare predicate...
            //
            struct comp : std::binary_function<value_type,value_type,bool>
            {
                bool operator()(const value_type &a, const value_type &b) const
                {
                    return a.first > b.first;
                }
            };

        public:
            base_heap()
            : _M_cont()
            {
                // std::make_heap()
            }

            ~base_heap()
            {} 

            void 
            push(const key_type &key, const mapped_type &value)
            {
                _M_cont.push_back(std::make_pair(key,value));
                std::push_heap(_M_cont.begin(), _M_cont.end(), comp());
            }

            value_type
            pop()
            {
                const value_type ret = _M_cont.front(); 
                std::pop_heap(_M_cont.begin(), _M_cont.end(), comp());
                _M_cont.pop_back();   
                return ret; 
            }

            mapped_type 
            pop_value()
            {
                V ret = _M_cont.front().second; 
                std::pop_heap(_M_cont.begin(), _M_cont.end(), comp());
                _M_cont.pop_back();   
                return ret; 
            }

            value_type &
            top() 
            { return _M_cont.front(); }

            const value_type &
            top() const
            { return _M_cont.front(); }

            bool
            empty() const
            { return _M_cont.empty(); } 

        };

        // template alias is not available...
        //

        template <typename K, typename V>
        struct vector_heap : public  base_heap<K, V, std::vector> {};

        template <typename K, typename V>
        struct deque_heap : public base_heap<K, V, std::deque> {};


        // std::priority_queue adapter...
        //

        template <typename K, typename V>
        struct priority_queue_heap 
        {        
        public:
            typedef K               key_type;
            typedef V               mapped_type;
            typedef std::pair<K,V>  value_type;

        private:
            // compare predicate...
            //
            struct comp : std::binary_function<value_type,value_type,bool>
            {
                bool operator()(const value_type &a, const value_type &b) const
                {
                    return a.first > b.first;
                }
            };

            std::priority_queue<value_type, std::vector<value_type>, comp>  _M_pq;

        public:
            priority_queue_heap()
            : _M_pq()
            {
                // std::make_heap()
            }

            ~priority_queue_heap()
            {} 

            void 
            push(const key_type &key, const mapped_type &value)
            {
                _M_pq.push(std::make_pair(key,value));
            }

            value_type
            pop()
            {
                const value_type ret = _M_pq.top(); 
                _M_pq.pop();   
                return ret; 
            }

            mapped_type 
            pop_value()
            {
                V ret = _M_pq.top().second; 
                _M_pq.pop();   
                return ret; 
            }

            value_type &
            top() 
            { return _M_pq.top(); }

            const value_type &
            top() const
            { return _M_pq.top(); }

            bool
            empty() const
            { return _M_pq.empty(); } 

        };
    }

    // SGI: A heap is a particular way of ordering the elements in a range [f, l)
    // this heap implementation is based on std::map<>. Basically it's heap
    // implemented by means of a redblack tree. 

    namespace redblack {

        template <typename K, typename V>
        class heap
        {        
        public:
            typedef K               key_type;
            typedef V               mapped_type;
            typedef std::pair<K,V>  value_type;

        private:
            std::map<key_type, mapped_type, std::less<K> >  _M_cont;

        public:
            heap()
            : _M_cont()
            {}

            ~heap()
            {} 

            void 
            push(const key_type &key, const mapped_type &value)
            {
                _M_cont.insert( std::make_pair(key,value) );
            }

            value_type    
            pop()
            {
                const value_type ret = * _M_cont.begin(); 
                _M_cont.erase(_M_cont.begin());
                return ret; 
            }

            V pop_value()
            {
                V ret = _M_cont.begin()->second; 
                _M_cont.erease(_M_cont.begin());
                return ret; 
            }

            value_type &
            top() 
            { return * _M_cont.begin(); }

            const value_type &
            top() const
            { return * _M_cont.begin(); }

            bool
            empty() const
            { return _M_cont.empty(); } 
 
        };
    }

} // namespace qrt

#endif /* _QRT_HEAP_HPP_ */
