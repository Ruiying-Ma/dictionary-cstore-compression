# include "OuterList.hpp"

std::pair<int, int> OuterList::left_right(InnerList* target_inner_list)
{
    int left = 0, right = 0;
    bool is_left = true;
    //int inner_list_cnt = 0;
    for(auto& inner_list : m_list) {
        assert(inner_list->get_outer_list_id() == m_id);
        // if (inner_list_cnt == 0 || inner_list_cnt == m_list.size() - 1) {
        //     assert(inner_list->get_size() <= inner_list->get_capacity());
        // } else {
        //     assert(inner_list_cnt > 0 && inner_list_cnt < m_list.size() - 1);
        //     assert(inner_list->is_full());
        // }
        if (is_left && inner_list != target_inner_list) {left += inner_list->get_size();}
        else if (inner_list == target_inner_list)   {is_left = false;}
        else {
            assert((!is_left) && (inner_list != target_inner_list));
            right += inner_list->get_size();
        }
        //inner_list_cnt++;
    }
    assert(is_left == false);
    assert(left + right + target_inner_list->get_size() == m_size);
    return std::make_pair(left, right);
}

void OuterList::push_back_data(std::string data)
{
    InnerList* inner_list = m_list.back();
    assert(inner_list->get_size() < inner_list->get_capacity());
    inner_list->push_back(data);
    m_size++;

}

void OuterList::push_front_data(std::string data)
{
    InnerList* inner_list = m_list.front();
    assert(inner_list->get_size() < inner_list->get_capacity());
    inner_list->push_front(data);
    m_size++;
}

void OuterList::push_back_inner_list(InnerList* inner_list)
{
    assert(back_inner_list_is_full());
    assert(inner_list->get_size() == 1);
    m_list.push_back(inner_list);
    inner_list->set_outer_list_id(m_id);
    m_size += inner_list->get_size();

}

void OuterList::push_front_inner_list(InnerList* inner_list)
{
    assert(front_inner_list_is_full());
    assert(inner_list->get_size() == 1);
    m_list.push_front(inner_list);
    inner_list->set_outer_list_id(m_id);
    m_size += inner_list->get_size();
}

void OuterList::push_back_in_order(OuterList &list)
{
    for(auto iter = list.m_list.begin(); iter != list.m_list.end(); iter++){
        m_list.push_back(*iter);
        m_list.back()->set_outer_list_id(m_id);
        m_size += m_list.back()->get_size();
    }
}

void OuterList::push_back_reverse_order(OuterList &list)
{
    for(auto iter = list.m_list.rbegin(); iter != list.m_list.rend(); iter++){
        m_list.push_back(*iter);
        m_list.back()->set_outer_list_id(m_id);
        m_size += m_list.back()->get_size();
    }
}

void OuterList::push_front_in_order(OuterList &list)
{
    for(auto iter = list.m_list.rbegin(); iter != list.m_list.rend(); iter++){
        m_list.push_front(*iter);
        m_list.front()->set_outer_list_id(m_id);
        m_size += m_list.front()->get_size();
    }
}

void OuterList::push_front_reverse_order(OuterList &list)
{
    for(auto iter = list.m_list.begin(); iter != list.m_list.end(); iter++){
        m_list.push_front(*iter);
        m_list.front()->set_outer_list_id(m_id);
        m_size += m_list.front()->get_size();
    }
}

