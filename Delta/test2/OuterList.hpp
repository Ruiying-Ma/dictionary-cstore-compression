# include "InnerList.hpp"

class OuterList {
    public:
    int get_size()  {return m_size;}
    int get_id()    {return m_id;}
    void increment_size(int delta = 1)   {m_size += delta;}
    InnerList* get_back_inner_list()    {assert(m_list.size() > 0); return m_list.back();}
    InnerList* get_front_inner_list()   {assert(m_list.size() > 0); return m_list.front();}
    void set_id(int id) {m_id = id;}

    bool back_inner_list_is_full()  {return m_list.size() == 0 || m_list.back()->is_full();}
    bool front_inner_list_is_full() {return m_list.size() == 0 || m_list.front()->is_full();}
    void push_back_data(std::string data);
    void push_front_data(std::string data);
    void push_back_inner_list(InnerList* inner_list);
    void push_front_inner_list(InnerList* inner_list);
    void push_back_in_order(OuterList &list);
    void push_back_reverse_order(OuterList &list);
    void push_front_in_order(OuterList &list);
    void push_front_reverse_order(OuterList &list);
    std::pair<int, int> left_right(InnerList* target_inner_list);

    std::list<InnerList*> m_list;

    private:
    int m_id = -1;
    int m_size = 0;
};