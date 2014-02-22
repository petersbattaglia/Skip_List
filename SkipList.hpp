#ifndef PBATTAG1_SKPLST_HPP
#define PBATTAG1_SKPLST_HPP
#define PROBABILITY 0.5

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

namespace cs540 
{
	template <typename Key_T, typename Mapped_T, size_t MaxLevel> 
	class SkipList
	{

		typedef std::pair<const Key_T, Mapped_T> ValueType;

		class Node
		{
			public:
				Node() : nodes(std::vector<Node*>(MaxLevel, NULL)) { }
				Node(ValueType data, size_t sz) : dataPair(data), nodes(std::vector<Node*>(MaxLevel, NULL)), size(sz) { }
				ValueType dataPair;
				std::vector<Node*> nodes;
				size_t size;
		};

		public:

			class BaseIterator
			{
				public:
					bool operator==(const BaseIterator &a) { return this == &a;  }
					bool operator!=(const BaseIterator &a) { return !(this == &a); }
			};

			class Iterator : public BaseIterator
			{
				public:
					Iterator(Node* item) : value(item) { }
					
					Iterator &operator++() 
					{ 
						value = value->nodes[0];
						return *this;
					}

					Iterator operator++(int) 
					{ 
						Iterator temp = *this;
						value = value->nodes[0];
						return temp;
					}

					ValueType &operator*() const 
					{ 
						return value->dataPair;
					}

					ValueType *operator->() const 
					{
						return &value->dataPair;
					}

					Node* value;
			};

			class ConstIterator : public BaseIterator
			{
				public:
					ConstIterator(Node* item) : value(item) { }
					
					ConstIterator &operator++() 
					{ 
						value = value->nodes[0];
						return *this;
					}

					ConstIterator operator++(int) 
					{ 
						ConstIterator temp = *this;
						value = value->nodes[0];
						return temp;
					}

					const ValueType &operator*() const 
					{ 
						return value->dataPair;
					}

					const ValueType *operator->() const 
					{
						return &value->dataPair;
					}
				
					Node* value;
			};

			SkipList() 
			{ 
				srand(time(NULL));
				levels = new Node();
				sz = 0;
			}

			SkipList(const SkipList &list) 
			{ 
				levels = new Node();
				sz = 0;

				Iterator temp = list.begin();
				for(int i = 0; i < list.size(); i++)
					insert((temp++).value);
			}

			SkipList &operator=(const SkipList &list) 
			{ 
				clear();
				Iterator temp = list.begin();
				for(int i = 0; i < list.size(); i++)
					insert((temp++).value);
			}

			size_t size() const 
			{ 
				return sz;
			}

			Iterator begin() 
			{ 
				return Iterator(levels->nodes[0]);
			}

			Iterator end() 
			{ 
				Node* temp = levels->nodes[0];
				while(temp != NULL) temp = temp->nodes[0];
				return Iterator(temp);
			}

			ConstIterator begin() const
			{ 
				return ConstIterator(levels->nodes[0]);
			}

			ConstIterator end() const
			{ 
				Node* temp = levels->nodes[0];
				while(temp != NULL) temp = temp->nodes[0];
				return ConstIterator(temp);
			}

			Iterator find(const Key_T &key) 
			{ 
				Node* current_node;
				Node* fallback_node = levels;

				for(int i = (MaxLevel - 1); i >= 0; i--)
				{
					current_node = fallback_node;
					while(current_node != NULL)
					{
						fallback_node = current_node;
						if(current_node->nodes[i] == NULL || current_node->nodes[i]->dataPair.first > key)
							break; // Next string on this node
						else if (current_node->nodes[i]->dataPair.first == key)
							return Iterator(current_node->nodes[i]);
						else if(current_node->nodes[i]->dataPair.first < key)
							current_node = current_node->nodes[i];
					}
				}
				return NULL;
			}
			
			ConstIterator find(const Key_T &key) const 
			{ 
				Node* current_node;
				Node* fallback_node = levels;

				for(int i = (MaxLevel - 1); i >= 0; i--)
				{
					current_node = fallback_node;
					while(current_node != NULL)
					{
						fallback_node = current_node;
						if(current_node->nodes[i] == NULL || current_node->nodes[i]->dataPair.first > key)
							break; // Next string on this node
						else if (current_node->nodes[i]->dataPair.first == key)
							return ConstIterator(current_node->nodes[i]);
						else if(current_node->nodes[i]->dataPair.first < key)
							current_node = current_node->nodes[i];
					}
				}
				return NULL;
			}
			
			std::pair<Iterator, bool> insert(ValueType &pair)
			{
				int top_level = getRandomLevel();
				std::stack<Node*> update_node_stack;
				Node* current_node;
				Node* fallback_node = levels;
				bool exists = false;

				for(int i = (MaxLevel - 1); i >= 0; i--)
				{
					current_node = fallback_node;
					while(current_node != NULL)
					{
						fallback_node = current_node;
						if(current_node->nodes[i] == NULL)
						{
							if(top_level > i) update_node_stack.push(current_node);
							break; // Next string on this node
						}
						else if (current_node->nodes[i]->dataPair.first == pair.first)
						{
							current_node->nodes[i]->dataPair.second = pair.second;
							exists = true;
							goto just_an_update; // Seems like the best way to get out
						}
						else if(current_node->nodes[i]->dataPair.first > pair.first)
						{
							if(top_level > i) update_node_stack.push(current_node);
							break; // Next string on this node
						}
						else if(current_node->nodes[i]->dataPair.first < pair.first)
						{
							current_node = current_node->nodes[i];
						}
					}
				}

				just_an_update:
				if(!exists)
				{
					Node *new_node = new Node(pair, top_level);
					if(fallback_node == NULL) // This is the first element -- Special Case
					{
						for(int k = 0; k < top_level; k++)
							levels->nodes[k] = new_node;
					}
					else
					{
						// ASSERT: Current Node points to a node who precedes our node to 
						// insert on at least string 0.
						int j = 0;
						Node *temp;
						while(update_node_stack.size() > 0)
						{
							// pop each node to be updated out [ 0 -> topLevel ]
							temp = update_node_stack.top()->nodes[j];
							update_node_stack.top()->nodes[j] = new_node;
							new_node->nodes[j] = temp;
							update_node_stack.pop();
							++j;
						}
					}
					++sz;
					return std::pair<Iterator, bool>(Iterator(new_node), exists);
				}
				else
				{
					return std::pair<Iterator, bool>(Iterator(fallback_node), exists);
				}
			}

			template <typename IT_T> 
			void insert(IT_T range_beg, IT_T range_end) 
			{ 
				for (IT_T iter = range_beg; iter != range_end; ++iter)
					insert (*iter);
			}
			
			void erase(Iterator pos) 
			{ 
				Node *org_val = pos.value;
				for(int i = 0; i < pos.value->size; i++)
				{
					Node* temp = levels->nodes[i];
					Node* prev = levels;
					while(temp != NULL && temp != org_val)
					{
						prev = temp;
						temp = temp->nodes[i];
					}

					if(prev!=NULL)
						prev->nodes[i] = org_val->nodes[i];
				}
				delete org_val;	
				--sz;
			}

			void erase(Iterator range_beg, Iterator range_end) 
			{ 
				Iterator temp = range_beg;
				Iterator to_delete;
				while(temp != range_end)
				{
					to_delete = temp;
					erase(temp);
					temp = ++to_delete;
				}
			}

			void clear() 
			{
				while(sz > 0)
					erase((*this).begin());
			}

			bool operator==(const SkipList &a) 
			{ 
				if(sz != a.size()) return false;

				Iterator b = a.begin();
				Node* temp = levels->nodes[0];
				for(int i = 0; i < sz; i++)
				{
					if(!(b.value->dataPair.first == temp->dataPair.first && 
						b.value->dataPair.second == temp->dataPair.second))
					{
						return false;
					}
					b++;
					temp = temp->nodes[0];
				}
				return true;
			}
			
			bool operator!=(const SkipList &a) 
			{ 
				return !(*this == a); 
			}

			bool operator<(const SkipList &a) 
			{ 
				Iterator b = a.begin();
				Node* temp = levels->nodes[0];
				for(int i = 0; i < sz; i++)
				{
					if(temp == NULL && b != NULL) return true;
					if(temp != NULL && b == NULL) return false;
					if(temp == NULL && b == NULL) return true;

					if(temp->dataPair.second < b.value->dataPair.second)
					{
						return true;
					}
					b++;
					temp = temp->nodes[0];
				}
				return true;
			}

			~SkipList()
			{
				Node* temp = levels->nodes[0];
				while(temp != NULL)
				{
					Node* old_temp = temp;
					temp = temp->nodes[0];
					delete old_temp;						
				}
				delete levels;
			}

		private:
			Node* levels;
			size_t sz;
			int getRandomLevel()
			{
				int retLevel = 1;
				while((((double) rand() / (RAND_MAX)) < PROBABILITY) && (retLevel < (MaxLevel)))
				{
					++retLevel;
				}
				return retLevel;
			}
	};
}

#endif
