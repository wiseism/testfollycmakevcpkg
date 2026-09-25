//
// Created by ibqo on 2019/12/30.
//

#include "gtest/gtest.h"
#include <iostream>
using namespace std;

void add(){

    pair <string,double> product1 ("tomatoes",3.25);
    pair <string,double> product2;
    pair <string,double> product3;

    product2.first ="lightbulbs"; // type of first is string
    product2.second =0.99; // type of second is double

    product3 = make_pair ("shoes",20.0);

    cout <<"The price of "<< product1.first <<" is $"<< product1.second <<"\n";
    cout <<"The price of "<< product2.first <<" is $"<< product2.second <<"\n";
    cout <<"The price of "<< product3.first <<" is $"<< product3.second <<"\n";
}

TEST(test1, c1){
    add();
}

GTEST_API_ int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
