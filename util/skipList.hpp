/**
 * @file skipList.hpp
 * @author yangyf83@aliyun.com
 * @brief 
 * @version 0.1
 * @date 2022-06-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
# pragma once

#include <vector>
#include <iostream>

using namespace std;

template <typename T>
struct SkipNode
{
    int key;
    T value;
    std::vector<SkipNode *> next;

    SkipNode(int k, T v, int level);
};

template <class T>
class SkipList
{

public:
    //头结点
    SkipNode<T> *head;

    //列表最大层数
    int maxLevel;

    //整型的最小值和最大值
    const int minInt = numeric_limits<int>::min();
    const int maxInt = numeric_limits<int>::max();

public:
    //构造函数
    SkipList(int maxLevel, T iniValue);

    //析构函数
    ~SkipList();

    //随机层数方法
    int randomLevel();

    //插入, 查找， 删除
    SkipNode<T> *insert(int k, T v);
    SkipNode<T> *find(int k);
    SkipNode<T> *deleteNode(int k);

    //打印
    void printNode();

private:
    //尾节点
    SkipNode<T> *tail;

    //找到当前列表或者node的最大层数
    int nodeLevel(vector<SkipNode<T> *> next);
};