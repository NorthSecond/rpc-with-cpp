/**
 * @file skipList.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "skipList.hpp"
#include <random>

using namespace std;

template <typename T>
SkipNode<T>::SkipNode(int k, T v, int level)
    : key(k), value(v)
{
    for (int i = 0; i < level; i++)
    {
        next.push_back(nullptr);
    }
}

//初始化
template <class T>
SkipList<T>::SkipList(int maxLevel, T iniValue)
    : maxLevel(maxLevel)
{
    //初始化头结点和尾节点为整型最小值和最大值
    head = new SkipNode<T>(minInt, iniValue, maxLevel);
    tail = new SkipNode<T>(maxInt, iniValue, maxLevel);

    //所有层数上的头结点指向尾节点
    for (int i = 0; i < maxLevel; i++)
    {
        head->next[i] = tail;
    }
}

template <class T>
SkipList<T>::~SkipList()
{
    delete head;
    delete tail;
}

template <class T>
int SkipList<T>::randomLevel()
{
    int random_level = 1;
    int seed = time(NULL);
    static std::default_random_engine e(seed);
    static std::uniform_int_distribution<int> u(0, 1);

    while (u(e) && random_level < maxLevel)
    {
        random_level++;
    }

    return random_level;
}

template <class T>
int SkipList<T>::nodeLevel(vector<SkipNode<T> *> next)
{
    int node_level = 0;

    if (next[0]->key == maxInt)
    {
        return node_level;
    }

    for (int i = 0; i < next.size(); i++)
    {
        if (next[i] != nullptr && next[i]->key != maxInt)
        {
            node_level++;
        }
        else
        {
            break;
        }
    }
    return node_level;
}

/*插入：
1）首先用查找函数来判断该结点是否已经存在，如果存在，则更新该结点的值
2）获取新节点的随机层数
3）找到合适的插入位置
4）插入，并调整每层前后node的指针*/
template <class T>
SkipNode<T> *SkipList<T>::insert(int k, T v)
{
    int x_level = randomLevel();
    SkipNode<T> *new_node = nullptr;
    SkipNode<T> *tmp = head;

    new_node = find(k);

    if (new_node)
    {

        new_node->value = v;
        cout << "\nThis node " << k << " has already existed. And its value has been updated to " << v << endl;
        return head;
    }

    cout << "key: " << k << ", randomLevel: " << x_level << endl;

    new_node = new SkipNode<T>(k, v, x_level);

    for (int i = (x_level - 1); i > -1; i--)
    {
        while (tmp->next[i] != nullptr && tmp->next[i]->key < k)
        {
            tmp = tmp->next[i];
        }
        new_node->next[i] = tmp->next[i];
        tmp->next[i] = new_node;
    }

    return head;
}

/*查找：
由于列表有序，首先找到小于该结点的最近的结点，如果下一个结点等于目标结点，则返回该节点。
如果不是，则返回空*/
template <class T>
SkipNode<T> *SkipList<T>::find(int x)
{
    SkipNode<T> *tmp = head;
    int current_level = nodeLevel(tmp->next);

    for (int i = (current_level - 1); i > -1; i--)
    {
        while (tmp->next[i] != nullptr && tmp->next[i]->key < x)
        {
            tmp = tmp->next[i];
        }
    }
    tmp = tmp->next[0];

    if (tmp->key == x)
    {
        return tmp;
    }
    else
    {
        return nullptr;
    }
}

/*删除：
1) 用 find(x) 方法判断该结点是否存在. 如果不存在，则返回当前list, 并告知该结点不存在。
2) 找到小于该结点的最近的结点。
3) 更改该节点每层的前面的结点的指针。*/
template <class T>
SkipNode<T> *SkipList<T>::deleteNode(int x)
{
    SkipNode<T> *node = find(x);
    if (!node)
    {
        return nullptr;
    }
    else
    {
        SkipNode<T> *tmp = head;
        int x_level = node->next.size();

        for (int i = (x_level - 1); i > -1; i--)
        {
            while (tmp->next[i] != nullptr && tmp->next[i]->key < x)
            {
                tmp = tmp->next[i];
            }
            tmp->next[i] = tmp->next[i]->next[i];
        }
        delete (node);
        return head;
    }
}

// 分层打印
template <class T>
void SkipList<T>::printNode()
{

    for (int i = 0; i < maxLevel; i++)
    {
        SkipNode<T> *tmp = head;
        int lineLen = 1;

        if (tmp->next[i]->key != maxInt)
        {
            cout << "\n";
            cout << "This is level " << i << ":" << endl;
            cout << "{";

            while (tmp->next[i] != nullptr && tmp->next[i]->key != maxInt)
            {
                cout << "("
                     << "Key: " << tmp->next[i]->key << ", ";
                cout << "Value: " << tmp->next[i]->value << ")";

                tmp = tmp->next[i];

                if (tmp->next[i] != nullptr && tmp->next[i]->key != maxInt)
                {
                    cout << ", ";
                }

                if (lineLen++ % 5 == 0)
                    cout << "\n";
            }
            cout << "}"
                 << "\n";
        }
    }
}

