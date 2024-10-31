# include "InnerList.hpp"

std::pair<int, int> InnerList::left_right(std::string target_data)
{
    int left = 0, right = 0;
    bool is_left = true;
    for(auto& data : m_list) {
        if (is_left && data != target_data) {left++;}
        else if (data == target_data) {is_left = false;}
        else {
            assert((!is_left) && (data != target_data));
            right++;
        }
    }
    assert(is_left == false);
    assert(left + right + 1 == m_list.size());
    return std::make_pair(left, right);
}

void InnerList::push_back(std::string data) 
{
    assert(m_list.size() < m_capacity);
    m_list.push_back(data);
}

void InnerList::push_front(std::string data) 
{
    assert(m_list.size() < m_capacity);
    m_list.push_front(data);
}