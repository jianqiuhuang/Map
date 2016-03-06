#ifndef MAP_HPP
#define MAP_HPP

#include <iostream>
#include <stdlib.h>
#include <time.h>

#define LEVELS 32
#define DEBUG 0 

namespace cs540{
template<typename K, typename V>
class Map{
        typedef std::pair<const K, V> ValueType;

        private:
            class NodeBase{
                friend class Map;
              public:
                NodeBase(){
                   this->next = new NodeBase*[LEVELS];
                   this->prev = new NodeBase*[LEVELS];
                   for(int i = LEVELS-1; i >= 0; --i){
                        this->next[i] = this;
                        this->prev[i] = this;
                   }
                }
                ~NodeBase(){
                    delete [] prev;
                    delete [] next;
                
                }
              private:
                NodeBase **next;
                NodeBase **prev;
            };
            class Node : public NodeBase{
                friend class Map;
                public:
                Node(const ValueType &data, int l): data(data){
                   this->levels = l;
                   this->next = new NodeBase*[l];
                   this->prev = new NodeBase*[l];
                }
                private:
                ValueType data;
                int levels;
            };

        public:
            class Iterator{
                friend class Map;
                friend class ConstIterator;
                public:
                    Iterator(){}
                    Iterator(NodeBase *n) : curr(n) {}
                    Iterator &operator++(){
                        curr = curr->next[0];
                        return *this;
                    }
                    Iterator operator++(int){
                        auto tmp = curr;
                        curr = curr->next[0];
                        return Iterator(tmp);
                    }
                    Iterator &operator--(){
                        curr = curr->prev[0];
                        return *this;
                    }
                    Iterator operator--(int){
                        auto tmp = curr;
                        curr = curr->prev[0];
                        return Iterator(tmp);
                    }
                    ValueType *operator->() const{
                        return &(((Node*)curr)->data);
                    }
                    ValueType &operator*() const{
                        return ((Node*)curr)->data;
                    }
                    friend bool operator==(const Iterator &it1, const Iterator &it2){
                        return it1.curr == it2.curr;
                    }
                    friend bool operator!=(const Iterator &it1, const Iterator &it2){
                        return !(it1 == it2);
                    }
                private:
                    NodeBase *curr;
            };
         public:
            class ConstIterator{
                friend class Map;
                friend class Iterator;
                public:
                    ConstIterator(){}
                    ConstIterator(NodeBase *n) : curr(n){}
                    ConstIterator &operator++(){
                        curr = curr->next[0];    
                        return *this;
                    }
                    ConstIterator operator++(int){
                        auto tmp = curr;
                        curr = curr->next[0];
                        return ConstIterator(tmp);
                    } 
                    ConstIterator &operator--(){
                        curr = curr->prev[0];
                        return *this;
                    }
                    ConstIterator operator--(int){
                        auto tmp = curr;
                        curr = curr->prev[0];
                        return ConstIterator(tmp);
                    }
                    const ValueType &operator*() const{
                        return ((Node*)curr)->data;
                    }
                    const ValueType *operator->() const{
                        return &(((Node*)curr)->data);
                    }
                    friend bool operator==(const ConstIterator &it1, const ConstIterator &it2){
                        return it1.curr == it2.curr;
                    }
                    friend bool operator!=(const ConstIterator &it1, const ConstIterator &it2){
                        return !(it1 == it2);
                    }
                    friend bool operator==(const Iterator *it1, const ConstIterator &it2){
                        return it1.curr == it2.curr;
                    }
                    friend bool operator!=(const Iterator *it1, const ConstIterator &it2){
                        return !(it1 == it2);;
                    }
                    friend bool operator==(const ConstIterator *it1, const Iterator &it2){
                        return it1.curr == it2.curr;
                    }
                    friend bool operator!=(const ConstIterator *it1, const Iterator &it2){
                        return !(it1 == it2);
                    }

                private:
                    NodeBase *curr;
            };
          public:
            class ReverseIterator{
                friend class Map;
                public:
                    ReverseIterator(){}
                    ReverseIterator(NodeBase *n): curr(n){}
                    ReverseIterator &operator++(){
                        curr = curr->prev[0];
                        return *this;
                    }
                    ReverseIterator operator++(int){
                        auto tmp = curr;
                        curr = curr->prev[0];
                        return ReverseIterator(tmp);
                    }
                    ReverseIterator &operator--(){
                        curr = curr->next[0];
                        return *this;
                    }
                    ReverseIterator operator--(int){
                        auto tmp = curr;
                        curr = curr->next[0];
                        return ReverseIterator(tmp);
                    }
                    ValueType &operator*() const{
                        return ((Node*)curr)->data;
                    }
                    ValueType *operator->() const{
                       return &(((Node*)curr)->data);
                    }
                    friend bool operator==(const ReverseIterator &it1, const ReverseIterator &it2){
                        return it1.curr == it2.curr;
                    }
                    friend bool operator!=(const ReverseIterator &it1, const ReverseIterator &it2){
                        return !(it1 == it2);
                    }
                private:
                    NodeBase *curr;
            };
        public:

            //LEVELS -> a macro defines the number of levels of the skip list
            //Sentinel is instantiated as a Node by the default constructor
            Map(){
                mapSize = 0;
                sentinel = new NodeBase();
                srand(time(NULL));
            } 

            Map(const Map &obj){
                mapSize = 0;
                sentinel = new NodeBase();
                srand(time(NULL));
                auto curr = obj.sentinel->next[0];
                while(curr != obj.sentinel){
                    this->add(((Node*)curr)->data);
                    curr = curr->next[0];
                }
            }

            Map(std::initializer_list<ValueType> l){
                mapSize = l.size();
                sentinel = new NodeBase();

                for(auto it = l.begin(); it != l.end(); ++it){
                    this->insert(*it);    
                }
                
            }
            Map &operator=(const Map &obj){
                if(DEBUG) std::cout << "in assignment operator" << std::endl;
                mapSize = 0;
                //clean up memory first
                auto curr = sentinel->next[0];
                auto toBeDeleted = curr;
                while(curr != sentinel){
                   curr = curr->next[0];
                   delete toBeDeleted;
                }
                delete sentinel;
                sentinel = new NodeBase();
                //deep copy nodes in obj
                curr = obj.sentinel->next[0];
                while(curr != obj.sentinel){
                    this->add(((Node*)curr)->data);
                    curr = curr->next[0];
                }
                return *this;
            }

            //delete memory allocated for the next pointers
            //delete memory allocated for the node itself
            ~Map(){
                if(DEBUG){
                    std::cout << "Destructor called" << std::endl;
                }

                auto curr = sentinel->next[0];
                auto toBeDeleted = curr;
                while(curr != this->sentinel){
                    curr = curr->next[0];
                    /*
                    if(DEBUG){
                    std::cout << "deleting element: " << toBeDeleted->data.first << "--" << toBeDeleted->data.second << std::endl;
                    }
                    */
                //delete [] toBeDeleted->next;
                delete toBeDeleted;
                toBeDeleted = curr;
                }
                delete sentinel;
            }
            
            size_t size() const{
                return this->mapSize;
            }

            Iterator begin(){
                return Iterator(sentinel->next[0]);
            }
            
            Iterator end(){
                return Iterator(sentinel);
            }

            ConstIterator begin() const{
                return ConstIterator(sentinel->next[0]);
            }

            ConstIterator end() const{
                return ConstIterator(sentinel);
            }

            ReverseIterator rbegin() const{
                return ReverseIterator(sentinel->prev[0]);
            }

            ReverseIterator rend() const{
                return ReverseIterator(sentinel);
            }

            Iterator find(const K &k){
                auto curr = sentinel;
                for(int i = LEVELS-1; i >= 0; i--){
                    for(; curr->next[i] != this->sentinel; curr = curr->next[i]){
                        if(((Node*)curr->next[i])->data.first == k){
                            return Iterator(curr->next[i]);    
                        }
                        if(k < ((Node*)curr->next[i])->data.first)
                            break;
                    }   
                }
                return Iterator(sentinel);
            }                   

            ConstIterator find(const K &k) const{
                auto curr = sentinel;
                for(int i = LEVELS-1; i >= 0; i--){
                    for(; curr->next[i] != this->sentinel; curr = curr->next[i]){
                        if(((Node*)curr->next[i])->data.first == k){
                            return ConstIterator(curr->next[i]);    
                        }
                        if(k < ((Node*)curr->next[i])->data.first)
                            break;
                    }   
                }
                return ConstIterator(sentinel);
            }

            V &at(const K &k){
                auto curr = sentinel;
                for(int i = LEVELS-1; i >= 0; i--){
                    for(; curr->next[i] != this->sentinel; curr = curr->next[i]){
                        if(((Node*)curr->next[i])->data.first == k){
                            return ((Node*)curr->next[i])->data.second;    
                        }
                        if(k < ((Node*)curr->next[i])->data.first)
                            break;
                    }   
                }
                throw std::out_of_range("key not found");
            }

            const V &at(const K &k) const{
                auto curr = sentinel;
                for(int i = LEVELS-1; i >= 0; i--){
                    for(; curr->next[i] != this->sentinel; curr = curr->next[i]){
                        if(((Node*)curr->next[i])->data.first == k){
                            return ((Node*)curr->next[i])->data.second;    
                        }
                        if(k < ((Node*)curr->next[i])->data.first)
                            break;
                    }   
                }
                throw std::out_of_range("key not found");
            }

            V &operator[](const K &k) const{
                try{
                    return this->at(k);
                }catch(const std::out_of_range &oor){
                    V v;
                    this->add(std::make_pair(k, v));
                    return this->at(k);
                }
            }

            std::pair<Iterator, bool> insert(const ValueType &data){
                auto it = find(data.first);
                //if the pair does not exist in current data structure
                if(it == this->end()){
                    int level = 1;
                    for(int i = rand(); (i&1) == 1; i >>= 1){
                        ++level;
                    }
                    /*           
                    if(DEBUG){
                        std::cout << "inserting: " << data.first << "--" << data.second << ", with leve: " << level << std::endl;
                    } 
                    */    
                    NodeBase *newNode = new Node(data, level);
                    NodeBase *curr = sentinel;
                    for(int i = LEVELS-1; i >= 0; i--){
                        for(; curr->next[i] != this->sentinel; curr = curr->next[i]){
                            if(data.first < ((Node*)curr->next[i])->data.first){
                                break;
                            }
                        }
                        if(i < level){
                            newNode->next[i] = curr->next[i];
                            newNode->prev[i] = curr;
                            newNode->next[i]->prev[i] = newNode;
                            curr->next[i] = newNode;
                        }
                    }
                    ++mapSize;
                    return std::pair<Iterator, bool>(Iterator(newNode), true);
                //pair already exist
                }else{
                    return std::pair<Iterator, bool>(it, false);
                }
            }
            
            template <typename IT_T>
            void insert(IT_T range_beg, IT_T range_end){
                for(auto it = range_beg; it != range_end; ++it){
                    this->insert(*it);
                }
            } 
            
            void erase(Iterator pos){
                for(int i = 0; i < ((Node*)pos.curr)->levels; i++){
                    pos.curr->prev[i]->next[i] = pos.curr->next[i];
                    pos.curr->next[i]->prev[i] = pos.curr->prev[i]; 
                }                       
                --mapSize;
                delete pos.curr;
            }
    
            void erase(const K &k){
                auto it = this->find(k);
                if(it != this->end()){
                    erase(it);
                }else{
                    throw std::out_of_range("cannot erase: key not exist");
                }
            }

            void clear(){
                auto curr = sentinel->next[0];
                auto toBeDeleted = curr;
                while(curr != sentinel){
                    curr = curr->next[0];
                    delete toBeDeleted;
                    toBeDeleted = curr;
                    --mapSize;
                }
                for(int i = 0; i < LEVELS; ++i){
                    sentinel->next[i] = sentinel;
                    sentinel->prev[i] = sentinel;
                }
            }

            friend bool operator==(const Map &mp1, const Map &mp2){
                if(mp1.size() != mp2.size())
                    return false;
                for(auto it1 = mp1.begin(), it2 = mp2.begin(); (it1 != mp1.end() && it2 != mp2.end()); ++it1, ++it2){
                    if(it1->first != it2->first || it1->second != it2->second){
                        return false;
                    }
                }
                return true;
            }
            
            friend bool operator!=(const Map &mp1, const Map &mp2){
                return !(mp1 == mp2);
            }

            friend bool operator<(const Map &mp1, const Map &mp2){
                for(auto it1 = mp1.begin(), it2 = mp2.begin(); (it1 != mp1.end() && it2 != mp2.end()); ++it1, ++it2){
                    if(*(it1) > *(it2))
                        return false;
                } 
                if(mp1.size() > mp2.size())
                  return false;
                return true;
            }
    private:        
            //this is an insertion helper funciton for internal usage only
            //# of levels is determined by a random int rand()
            void add(const ValueType &data){

                int level = 1;

                //count the number of consecutive one's
                //use the count as the number of levels
                //since int is 32-bits (on remote), level can be 32 at most
                for(int i = rand(); (i&1) == 1; i >>= 1){
                    level++;
                }
                /* 
                if(DEBUG){
                    std::cout << "adding: " << data.first << "--" << data.second << ", with level = " << level << std::endl;
                }
                */

                NodeBase *newNode = new Node(data, level);
                NodeBase *curr = sentinel;
                for(int i = LEVELS-1; i >= 0; i--){
                    for(; curr->next[i] != this->sentinel; curr = curr->next[i]){
                        if(data.first < ((Node*)curr->next[i])->data.first){
                            break;
                        }
                    }
                    if(i < level){
                        newNode->next[i] = curr->next[i];
                        newNode->prev[i] = curr;
                        newNode->next[i]->prev[i] = newNode;
                        curr->next[i] = newNode;
                    }
                }
                ++mapSize;
            }
    private:
            size_t mapSize;
            NodeBase *sentinel;
};
}
#endif
