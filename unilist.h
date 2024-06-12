#ifndef unilist
#define unilist

#include <iostream>
#include <stdexcept>

template <typename item>
class Unilist;

template <typename unitType>
std::ostream& operator<<(std::ostream& out, Unilist<unitType>& obj);

template <typename item>
class Unilist
{
private:
	struct node
	{
		item* content;
		node* next;
	};

	node* head;
	node* tail;
	node* current;
	int size;

public:
	Unilist();
	Unilist(const item* sequence, int seq_size);
	Unilist(const Unilist<item>& orig);
	~Unilist();

	int get_size() { return size; }
	void add_elem(item* content, int index);
	void add_elem(item* content);
	void rm_elem(int index);
	void rm_elem(item* content);
	bool is_empty();


	Unilist<item>& operator=(const Unilist<item>& orig);
	item& operator[](int index);


	friend std::ostream& operator<<(std::ostream& out, Unilist<item>& obj);
};


template <typename item>
Unilist<item>::Unilist()
{
	head = nullptr;
	tail = nullptr;
	current = nullptr;
	size = 0;
}

template <typename item>
Unilist<item>::Unilist(const item* sequence, int seq_size)
{
	size = seq_size;
	for (int i = 0; i < size; i++)
	{
		if (!head)
		{
			head = new node;
			tail = head;
		}
		else
		{
			tail->next = new node;
			tail = tail->next;
		}
		tail->content = new item;
		*(tail->content) = sequence[i];
		tail->next = nullptr;
	}
}

template <typename item>
Unilist<item>::Unilist(const Unilist<item>& orig)
{
	size = orig.size;
	current = orig.head;
	while (current != nullptr)
	{
		if (!head)
		{
			head = new node;
			tail = head;
		}
		else
		{
			tail->next = new node;
			tail = tail->next;
		}
		tail->content = new item;
		*tail->content = *current->content;
		tail->next = nullptr;

		current = current->next;
	}
}

template <typename item>
Unilist<item>::~Unilist()
{
	if (size)
	{
		current = head;
		while (current != nullptr)
		{
			current = current->next;
			delete head->content;
			delete head;
			size--;
			head = current;
		}
	}
}

template<typename item>
void Unilist<item>::add_elem(item* content, int index)
{
	using std::cout;
	using std::endl;

	if (index > size || index < 0)
		throw std::range_error("index is out of range.");
	else
	{
		node* buffer = nullptr;
		if (!index)
		{
			buffer = head;
			head = new node;
			head->content = new item;
			*(head->content) = *content;
			head->next = buffer;
		}
		else
		{
			current = head;
			for (int i = 0; i < index - 1; i++)
				current = current->next;
			buffer = current->next;
			current->next = new node;
			current = current->next;
			current->content = new item;
			*(current->content) = *content;
			current->next = buffer;
			if (index == size - 1)
				tail = buffer;
		}
		size++;
	}
	return;
}

template<typename item>
void Unilist<item>::add_elem(item* content)
{
	if (!size)
	{
		head = new node;
		tail = head;
	}
	else
	{
		tail->next = new node;
		tail = tail->next;
	}
	tail->content = new item;
	*(tail->content) = *content;
	tail->next = nullptr;

	size++;

	return;
}

template <typename item>
void Unilist<item>::rm_elem(int index)
{
	using std::cout;
	using std::endl;

	if (index >= size || index < 0)
		throw std::range_error("index is out of range.");
	else
	{
		if (!index)
		{
			current = head->next;
			delete head->content;
			delete head;
			head = current;
		}
		else
		{
			node* buffer = nullptr;
			node* closing = nullptr;
			current = head;
			for (int i = 0; i < index - 1; i++)
				current = current->next;

			buffer = current->next;
			if (buffer == tail)
			{
				tail = current;
			}
			else
			{
				current->next = buffer->next;
			}

			delete buffer->content;
			delete buffer;
			
		}
		size--;
		return;
	}
}

template <typename item>
void Unilist<item>::rm_elem(item* content)
{
	using std::cout;
	using std::endl;

	if (size)
	{
		current = head->next;
		if (*(head->content) == *content)
		{
			delete head->content;
			delete head;
			head = current;
		}
		else
		{
			node* buffer = head;
			for (int i = 1; i < size; i++)
			{
				if (*(current->content) == *content)
				{
					buffer->next = current->next;
					delete current->content;
					delete current;
					break;
				}
				current = current->next;
				buffer = buffer->next;
			}
		}
		size--;
	}
	return;
}

template <typename item>
bool Unilist<item>::is_empty()
{
	if (!size)
		return true;
	else
		return false;
}

template <typename item>
Unilist<item>& Unilist<item>::operator=(const Unilist<item>& orig)
{
	if (this == &orig)
	{
		if (size)
		{
			current = head;
			while (current != nullptr)
			{
				current = current->next;
				delete head;
				head = current;
			}
		}

		current = orig.head;
		while (current != nullptr)
		{
			if (!head)
			{
				head = new node;
				tail = head;
			}
			else
			{
				tail->next = new node;
				tail = tail->next;
			}
			tail->content = current->content;
			tail->next = nullptr;

			current = current->next;
		}

	}

	return *this;
}

template <typename item>
item& Unilist<item>::operator[](int index)
{
	if (index >= size || index < 0)
		throw std::range_error("index is out of range.");
	else
	{
		current = head;
		for (int i = 0; i < index; i++)
			current = current->next;
		return *(current->content);
	}
}

template <typename unitType>
std::ostream& operator<<(std::ostream& out, Unilist<unitType>& obj)
{
	obj.current = obj.head;
	int acc = 1;
	while (obj.current != nullptr)
	{
		out << " #" << acc++ << ": " << *(obj.current->content) << std::endl;
		obj.current = obj.current->next;
	}

	return out;
}

#endif