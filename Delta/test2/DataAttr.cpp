# include "DataAttr.hpp"

std::pair<int, int> DataAttr::left_right(OuterList &outer_list)
{
    std::pair<int, int> outer_left_right = outer_list.left_right(m_inner_list);
    std::pair<int, int> inner_left_right = m_inner_list->left_right(m_data);
    int left = outer_left_right.first + inner_left_right.first;
    int right = outer_left_right.second + inner_left_right.second;
    assert(left + right + 1 == outer_list.get_size());
    return std::make_pair(left, right);
}

void DataAttr::push_back(OuterList& outer_list, int capacity)
{
    if (outer_list.back_inner_list_is_full()) {
        // new inner_list
        InnerList* new_inner_list = new InnerList(capacity, outer_list.get_id());
        new_inner_list->push_back(m_data);
        // update outer_list
        outer_list.push_back_inner_list(new_inner_list);
        // update DataAttr
        m_inner_list = new_inner_list;
    } else {
        // update outer_list
        outer_list.push_back_data(m_data);
        // update DataAttr
        m_inner_list = outer_list.get_back_inner_list();
    }
}

void DataAttr::push_front(OuterList& outer_list, int capacity)
{
    if (outer_list.front_inner_list_is_full()) {
        // new inner_list
        InnerList* new_inner_list = new InnerList(capacity, outer_list.get_id());
        new_inner_list->push_front(m_data);
        // update outer_list
        outer_list.push_front_inner_list(new_inner_list);
        // update DataAttr
        m_inner_list = new_inner_list;
    } else {
        // update outer_list
        outer_list.push_front_data(m_data);
        // update DataAttr
        m_inner_list = outer_list.get_front_inner_list();
    }
}