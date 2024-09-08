/*
g++ -std=c++20 -I./include test_ART_delete_03.cpp  artkey.cpp  node.cpp art.cpp node4.cpp node16.cpp node48.cpp node256.cpp prefix.cpp fixed_size_allocator.cpp leaf.cpp  value.cpp -o test_ART_delete_03.exe
*/

#include <iostream>
#include <optional>
#include <vector>

#include "art.hpp"
#include "artkey.hpp"
#include "common.hpp"
#include "leaf.hpp"
#include "logger.hpp"
#include "node.hpp"
#include "node256.hpp"
#include "node4.hpp"
#include "node48.hpp"
#include "string_type.hpp"
#include "value.hpp"

using namespace duckart;
// Global logger instance
Logger g_logger("duckart.log", Logger::DEBUG);

int main() {
    LOG_INFO("--------new round--------------");

    ART art;
    Node node;

    // key
    ARTKey key1 =
        ARTKey::CreateARTKey<string_t>("123456789eee");  // test case 2
    key1.Print();

    // leaf
    Node v_node;
    reference<Node> ref_leaf(v_node);
    string_t test_string1("Hello, World!");
    uint32_t  test_uint32 = static_cast<uint32_t >(1234567);
    std::cout << "test_uint16:"<< test_uint32 << std::endl;
    Value value1 =  Value::CreateValue(test_uint32);
    Leaf& leaf = Leaf::New(art, ref_leaf, value1);

    node = art.root.get();
    std::cout << "before insert: Node type:" << static_cast<int>(node.getTag())
              << std::endl;

    auto s = art.Insert(node, key1, v_node, 0);
    std::cout << s << std::endl;
    std::cout << "after insert: Node type:" << static_cast<int>(node.getTag())
              << std::endl;

    auto s1 = art.Insert(node, key1, v_node, 0);

 
    // key2
    ARTKey key2 =
        ARTKey::CreateARTKey<string_t>("123456789aaaa");  // test case 2

    // leaf2
    Node lnode2;
    reference<Node> ref_lnode2(lnode2);
    string_t test_string2("Hello, 1234567890!");
    Value value2 =  Value::CreateValue(test_string2);
    Leaf& leaf_node_2 =
        Leaf::New(art, ref_lnode2, value2);
    auto s2 = art.Insert(node, key2, ref_lnode2, 0);

    // key3
    ARTKey key3 =
        ARTKey::CreateARTKey<string_t>("123456789bbbb");  // test case 2
    key3.Print();

    // leaf3
    Node lnode3;
    reference<Node> ref_lnode3(lnode3);
    string_t test_string3("Hello, 1234567890!");
    Value value3 =  Value::CreateValue(test_string3);
    Leaf& leaf_node_3 =
        Leaf::New(art, ref_lnode3, value3);
    auto s3 = art.Insert(node, key3, ref_lnode3, 0);

    // key4
    ARTKey key4 =
        ARTKey::CreateARTKey<string_t>("123456789cccc");  // test case 2
    key4.Print();

    // leaf4
    Node lnode4;
    reference<Node> ref_lnode4(lnode4);
    string_t test_string4("Hello, 1234567890!");
    Value value4 =  Value::CreateValue(test_string4);
    Leaf& leaf_node_4 =
        Leaf::New(art, ref_lnode4, value4);
    auto s4 = art.Insert(node, key4, ref_lnode4, 0);

    // key5
    ARTKey key5 =
        ARTKey::CreateARTKey<string_t>("123456789dddd");  // test case 2
    key5.Print();

    // leaf5
    Node lnode5;
    reference<Node> ref_lnode5(lnode5);
    string_t test_string5("Hello, 1234567890!");
    Value value5 =  Value::CreateValue(test_string5);
    Leaf& leaf_node_5 =
        Leaf::New(art, ref_lnode5, value5);
    auto s5 = art.Insert(node, key5, ref_lnode5, 0);


     // key6
    ARTKey key6 =
        ARTKey::CreateARTKey<string_t>("623456789mmmm");  // test case 2
    key6.Print();

    // leaf6
    Node lnode6;
    reference<Node> ref_lnode6(lnode6);
    string_t test_string6("Hello, 1234567890!");
    Value value6 =  Value::CreateValue(test_string6);
    Leaf& leaf_node_6 =
        Leaf::New(art, ref_lnode6, value6);
    auto s6 = art.Insert(node, key6, ref_lnode6, 0);

    // print Node 
    std::cout << "-------------after insert-------------" << std::endl;
    Node::Print(art, node);

     //search   
    auto s_node = art.Search(node,key1,0);
    auto& s_leaf= Node::RefMutable<Leaf>(art, s_node, NType::LEAF);
    //string_t
    //std::cout <<"value of s_leaf:" << Value::ExtractValue<string_t>(s_leaf.value).GetData()  << std::endl;
    //uint16_t
    std::cout <<"value of s_leaf:" <<  Value::ExtractValue<uint32_t>(s_leaf.value)   << std::endl;
   

    // delete
    auto bool_flag = art.Delete(node, key1, 0);
    std::cout << "-------------after delete 1-------------"<< bool_flag << std::endl;
    Node::Print(art, node);
    bool_flag = art.Delete(node, key2, 0);
    std::cout << "-------------after delete 2-------------"<< bool_flag << std::endl;
    Node::Print(art, node);
    bool_flag = art.Delete(node, key3, 0);
    std::cout << "-------------after delete 3-------------"<< bool_flag << std::endl;
    Node::Print(art, node);

    bool_flag = art.Delete(node, key4, 0);
    std::cout << "-------------after delete 4-------------" << bool_flag<< std::endl;
    Node::Print(art, node); 

    bool_flag = art.Delete(node, key5, 0);
    std::cout << "-------------after delete 5-------------" << bool_flag<< std::endl;
    Node::Print(art, node); 

    bool_flag = art.Delete(node, key6, 0);
    std::cout << "-------------after delete 6-------------"<< bool_flag << std::endl;
    Node::Print(art, node); 
    

    return 1;
}
