#include <bits/stdc++.h>

class InnerList {
    public:
    InnerList(int capacity, int id): m_capacity(capacity), m_outer_list_id(id) {}
    int get_size()  {return m_list.size();}
    int get_capacity()  {return m_capacity;}
    int get_outer_list_id() {return m_outer_list_id;}
    void set_outer_list_id(int id) {m_outer_list_id = id;}
    bool is_full()  {return m_list.size() == m_capacity;}

    void push_front(std::string data);
    void push_back(std::string data);
    std::pair<int, int> left_right(std::string target_data);
    
    std::list<std::string> m_list;

    private:
    int m_outer_list_id = -1;
    int m_capacity = -1;
};