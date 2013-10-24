/*
 This contains a demo of the Ordered_list template, showing copy and assignment at work, 
 and how it is constructed and searched for a list of const ints, int * pointers, char * pointers, 
 and Thing objects, using different function object classes to supply a variety of ordering functions.
 Some templated functions are used to output the contents of lists of different types.
 
 Use a simple program like this as a "test harness" to systematically test your Ordered_list
 class functions, starting with the simplest and most basic.
 */
#include "Ordered_list.h"
#include "p2_globals.h"

// comment

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

// Things have an ID number used to compare them.
class Thing {
public:
	Thing(int i_in) : i(i_in) {}
	int get_ID() const 
		{return i;}
	bool operator< (const Thing& rhs) const
		{return i < rhs.i;}
	void increment()
		{i++;}
	friend ostream& operator<< (ostream& os, const Thing& t);
private:
	int i;
};

/* function prototypes */
ostream& operator<< (ostream& os, const Thing& t);

void demo_with_const_int();
void demo_copy_and_assignment();
Ordered_list<int> subfunction(Ordered_list<int>);
void demo_with_int_ptr();
void demo_with_cstr_ptr();
void demo_with_Things_default_comparison();
void demo_with_Things_custom_comparison();
void test_apply_if_with_Things();
/* declarations of functions for printing things out using apply */
void print_int(int i);
void print_int_char(int i, char c);

template<typename T, typename OF>
void print(const Ordered_list<T, OF>& in_list); 

template<typename T, typename OF>
void print_ptr(const Ordered_list<T, OF>& in_list); 

/* functions for demonstrating apply_if */
bool match_Thing2(const Thing& t);
bool match_Thing_int(const Thing& t, int i);


int main(void)
{
	demo_with_const_int();
	demo_copy_and_assignment();
	demo_with_int_ptr();
	demo_with_cstr_ptr();
	demo_with_Things_default_comparison();
	demo_with_Things_custom_comparison();
	test_apply_if_with_Things();
	
	cout << "Done!" << endl;
	return 0;
}

void demo_with_const_int()
{	
	cout << "\ndemo_with_const_int" << endl;
	// default < is used for ordering
	Ordered_list<const int> int_list;
		
	int_list.insert(3); 
	cout << "int_list size is " << int_list.size() << endl;
	print(int_list);
	
	int_list.insert(1); 
	cout << "int_list size is " << int_list.size() << endl;
	print(int_list);
	
	int_list.insert(2); 
	cout << "int_list size is " << int_list.size() << endl;
	print(int_list);
	
	apply(int_list.begin(), int_list.end(), print_int);
	apply_arg(int_list.begin(), int_list.end(), print_int_char, ':');
	cout << endl;
	
	// try out the find function
	for(int times = 0; times < 3; ++times) {
		cout << "Enter an int:";
		int i;
		cin >> i;
		Ordered_list<const int>::Iterator it = int_list.find(i);
		if(it == int_list.end())
			cout << "Not found" << endl;
		else {
			cout << "Found" << endl;
			// *it = 42;	// try to change it - compile error!
			}
	}
	
	int_list.clear();
	cout << "int_list size is " << int_list.size() << endl;
}	

void demo_copy_and_assignment()
{	
	cout << "\ndemo_copy_and_assignment" << endl;
	// default < is used for ordering
	Ordered_list<int> int_list;
		
	int_list.insert(3); 
	int_list.insert(1); 
	int_list.insert(2); 
	cout << "int_list size is " << int_list.size() << endl;
	print(int_list);
	
	// copy ctor
	Ordered_list<int> int_list2(int_list);
	cout << "copy ctor: int_list2 size is " << int_list2.size() << endl;
	print(int_list2);

	// assignment operator
	Ordered_list<int> int_list3;
	int_list3 = int_list;
	cout << "assignment: int_list3 size is " << int_list3.size() << endl;
	print(int_list3);
	
	// use in a function call and return by value
	int_list3 = subfunction(int_list);
	cout << "call-return by value result: int_list3 size is " << int_list3.size() << endl;
	print(int_list3);	
}	
	
Ordered_list<int> subfunction(Ordered_list<int> in_list)
{
	in_list.insert(4);
	in_list.insert(0);
	return in_list;
}
	
// function object class used to compare pointers to ints
// This is redundant with the Less_than_ptr template. 
struct Compare_int_ptrs
{
	bool operator() (const int * p1, const int * p2) const 
		{
			return *p1 < *p2;
		}
};

void demo_with_int_ptr()
{
	cout << "\ndemo_with_int_ptr" << endl;
	// build an ordered list of  int pointer,
	// using a function for the ordering - we want to compare the pointed-to numbers,
	// not the addresses in the pointers.
	// type of object in list is int *, type of ordering function is compare_int_ptrs
	typedef Ordered_list<int *, Compare_int_ptrs> int_ptr_list_t;
	int_ptr_list_t int_ptr_list;	// T is pointer to  int
	int i1 = 1; int i2 = 2; int i3 = 3;	// need variables that have addresses
	
	int_ptr_list.insert(&i3); 	
	int_ptr_list.insert(&i1); 	
	int_ptr_list.insert(&i2); 
	cout << "int_ptr_list size is " << int_ptr_list.size() << endl;
	print_ptr(int_ptr_list);
	
	// try out the find function
	for(int times = 0; times < 3; ++times) {
		cout << "Enter an int:";
		int i;
		cin >> i;
		// notice how we have to use the address of i as the "probe"
		int_ptr_list_t::Iterator it = int_ptr_list.find(&i);
		if(it == int_ptr_list.end())
			cout << "Not found" << endl;
		else {
			cout << "Found" << endl;
			*(*it) = 42;	// try to change it - but can disorder the list!
			print_ptr(int_ptr_list);
			cout << "did you really want to do that?" << endl;
			}
	}
}

// this function object class is defined to return true only if the first C-string
// compares less-than (comes before) the second.
struct Compare_c_string_ptrs 
{
	bool operator() (const char * p1, const char * p2) const
		{
			return (strcmp(p1, p2) < 0);
		}
};


void demo_with_cstr_ptr()
{
	cout << "\ndemo_with_cstr_ptr" << endl;
	/* Now a list of C string pointers. Build a list of s1, s2, s3 but insert in this order: s3, s1, s2,
	 and see if the strings get put in the right order. Use the compare_c_string_ptrs
	 function to compare the C-strings at the addresses in the pointers, instead of the pointers themselves. */
	
	const char * s1 = "s1";
	const char * s2 = "s2";
	const char * s3 = "s3";
	
	typedef Ordered_list<const char *, Compare_c_string_ptrs> cp_list_t;
	cp_list_t cp_list; // T is const char *
	cp_list.insert(s3);
	cp_list.insert(s1);
	cp_list.insert(s2);
	cout << "cp_list size is " << cp_list.size() << endl;
	print(cp_list);
	
	// try out the find function using a std::string for the input
	for(int times = 0; times < 3; ++times) {
		cout << "Enter a string:";
		string s;
		cin >> s;
		cp_list_t::Iterator it = cp_list.find(s.c_str());
		if(it == cp_list.end())
			cout << "Not found" << endl;
		else
			cout << "Found" << endl;
	}
	
	cp_list.clear();
	cout << "cp_list size is " << cp_list.size() << endl;
}	
	
void demo_with_Things_default_comparison()
{
	cout << "\ndemo_with_Things_default_comparison" << endl;
	/* now a list of Thing objects - each Thing constructed with an ID number */
	Thing t1(1);
	Thing t2(2);
	Thing t3(3);
	
	// use default less-than function
	typedef Ordered_list<Thing> Thing_list_t;
	
	Thing_list_t thing_list;
	
	thing_list.insert(t3);
	thing_list.insert(t1);
	thing_list.insert(t2);
	cout << "thing_list size is " << thing_list.size() << endl;
	print(thing_list);
	
	// try out the find function
	for(int times = 0; times < 3; ++times) {
		cout << "Enter an int:";
		int i;
		cin >> i;
		// We have to search for the same kind of thing that is in the list, 
		// so we need to construct a "probe" object to use in the search.
		Thing probe(i);
		Thing_list_t::Iterator it = thing_list.find(probe);
		if(it == thing_list.end())
			cout << "Not found" << endl;
		else {
			cout << "Found - will remove" << endl;
			thing_list.erase(it);
			cout << "thing_list size is " << thing_list.size() << endl;
			print(thing_list);
			}
	}
	
	// call a member function on each Thing using the iterator arrow operator
	for(Thing_list_t::Iterator it = thing_list.begin(); it != thing_list.end(); it++) {
		it->increment();
	}
	cout << "incremented thing_list" << endl;
	print(thing_list);
	
	thing_list.clear();
	cout << "thing_list size is " << thing_list.size() << endl;
}	


// put Things in reverse order with this function object class
struct Compare_Things_rev
{
	bool operator() (const Thing& t1, const Thing& t2) const
	{
		// compare in reverse order
		return t2 < t1;
	}
};

void demo_with_Things_custom_comparison()
{
	cout << "\ndemo_with_Things_custom_comparison" << endl;
	Thing t1(1);
	Thing t2(2);
	Thing t3(3);
	
	typedef Ordered_list<Thing, Compare_Things_rev> Thing_list_t;
	
	Thing_list_t thing_list;

	cout << "thing_list size is " << thing_list.size() << endl;
	
	thing_list.insert(t3);
	cout << "thing_list size is " << thing_list.size() << endl;
	print(thing_list);
	
	thing_list.insert(t1);
	cout << "thing_list size is " << thing_list.size() << endl;
	print(thing_list);
	
	thing_list.insert(t2);
	cout << "thing_list size is " << thing_list.size() << endl;
	print(thing_list);
	
	// try out the find function
	for(int times = 0; times < 3; ++times) {
		cout << "Enter an int:";
		int i;
		cin >> i;
		// We have to search for the same kind of thing that is in the list, 
		// so we need to construct a "probe" object to use in the search.
		Thing probe(i);
		Thing_list_t::Iterator it = thing_list.find(probe);
		if(it == thing_list.end())
			cout << "Not found" << endl;
		else {
			cout << "Found - will remove" << endl;
			thing_list.erase(it);
			cout << "thing_list size is " << thing_list.size() << endl;
			print(thing_list);
			}
	}
	
	// call a member function on each Thing using the iterator arrow operator
	for(Thing_list_t::Iterator it = thing_list.begin(); it != thing_list.end(); it++) {
		it->increment();
	}
	cout << "incremented thing_list" << endl;
	print(thing_list);
	
	thing_list.clear();
	cout << "thing_list size is " << thing_list.size() << endl;
}

void test_apply_if_with_Things()
{
	cout << "\ntest_apply_if_with_Things" << endl;
	/* a list of Thing objects - each Thing constructed with an ID number */
	Thing t1(1);
	Thing t2(2);
	Thing t3(3);
	
	typedef Ordered_list<Thing> Thing_list_t;
	
	Thing_list_t thing_list; // use default less-than function
	
	thing_list.insert(t3);	
	thing_list.insert(t1);
	thing_list.insert(t2);
	cout << "thing_list size is " << thing_list.size() << endl;
	print(thing_list);
	
	// try out some apply-if functions
	// use apply_if to see if a specific Thing is in the container
	if(apply_if(thing_list.begin(), thing_list.end(), match_Thing2)) {
		cout << "there is a match for Thing2" << endl;
		}

	// use apply_if arg similarly using an entered value
	cout << "Enter an int:";
	int i;
	cin >> i;
	if(apply_if_arg(thing_list.begin(), thing_list.end(), match_Thing_int, i)) {
		cout << "there is a match for the value " << i << endl;
		}
}	

bool match_Thing2(const Thing& t)
{
	return (t.get_ID() == 2);
}

bool match_Thing_int(const Thing& t, int i)
{
	return (t.get_ID() == i);
}

void print_int(int i)
{
	cout << i << endl;
}

void print_int_char(int i, char c)
{
	cout << i << c;
}

// Print the contents of the list on one line, separated by spaces.
// This requires that operator<< be defined for the type of item in the list.
template<typename T, typename OF>
void print(const Ordered_list<T, OF>& in_list)
{
	for(typename Ordered_list<T, OF>::Iterator it = in_list.begin(); it != in_list.end(); it++) {
		if(it != in_list.begin())	// output a leading space after the first one
			cout << ' ';
		cout << *it;
	}
	cout << endl;
}

template<typename T, typename OF>
void print_ptr(const Ordered_list<T,OF>& in_list)
{
	for(typename Ordered_list<T, OF>::Iterator it = in_list.begin(); it != in_list.end(); it++) {
		if(it != in_list.begin())	// output a leading space after the first one
			cout << ' ';
		cout << *(*it);
	}
	cout << endl;
}

ostream& operator<< (ostream& os, const Thing& t)
{
	os << t.i;
	return os;
}

/* Sample output
demo_with_const_int
int_list size is 1
3
int_list size is 2
1 3
int_list size is 3
1 2 3
1
2
3
1:2:3:
Enter an int:3
Found
Enter an int:5
Not found
Enter an int:0
Not found
int_list size is 0

demo_copy_and_assignment
int_list size is 3
1 2 3
copy ctor: int_list2 size is 3
1 2 3
assignment: int_list3 size is 3
1 2 3
call-return by value result: int_list3 size is 5
0 1 2 3 4

demo_with_int_ptr
int_ptr_list size is 3
1 2 3
Enter an int:2
Found
1 42 3
did you really want to do that?
Enter an int:3
Not found
Enter an int:1
Found
42 42 3
did you really want to do that?

demo_with_cstr_ptr
cp_list size is 3
s1 s2 s3
Enter a string:xx
Not found
Enter a string:s1
Found
Enter a string:s3
Found
cp_list size is 0

demo_with_Things_default_comparison
thing_list size is 3
1 2 3
Enter an int:2
Found - will remove
thing_list size is 2
1 3
Enter an int:5
Not found
Enter an int:1
Found - will remove
thing_list size is 1
3
incremented thing_list
4
thing_list size is 0

demo_with_Things_custom_comparison
thing_list size is 0
thing_list size is 1
3
thing_list size is 2
3 1
thing_list size is 3
3 2 1
Enter an int:5
Not found
Enter an int:0
Not found
Enter an int:2
Found - will remove
thing_list size is 2
3 1
incremented thing_list
4 2
thing_list size is 0

test_apply_if_with_Things
thing_list size is 3
1 2 3
there is a match for Thing2
Enter an int:3
there is a match for the value 3
Done!
 */

