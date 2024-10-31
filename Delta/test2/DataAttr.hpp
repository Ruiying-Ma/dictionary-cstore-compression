#include "OuterList.hpp"

class DataAttr {
    public:
    DataAttr(std::string s): m_data(s), m_front_freq(1), m_inner_list(nullptr) {}
    std::string get_data()  {return m_data;}
    int get_front_freq()    {return m_front_freq;}
    int get_dict()  {return m_dict;}
    InnerList* get_inner_list() {return m_inner_list;}
    int get_ourter_list_id()    {assert(m_inner_list != nullptr); return m_inner_list->get_outer_list_id();}
    void increment_front_freq() {m_front_freq++;}
    void set_dict(int dict) {m_dict = dict;}

    std::pair<int, int> left_right(OuterList &outer_list);
    void push_back(OuterList& outer_list, int capacity);
    void push_front(OuterList& outer_list, int capacity);


    private:
    int m_front_freq = -1;
    int m_dict = -1;
    std::string m_data;
    InnerList* m_inner_list;
};