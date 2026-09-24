#include "CustomVector.h"      // includes std::alignof

int main() {
    // initializer list construction
    vector<std::string> vec = { "hello", "i", "am", "luke" };

    // initializer list assignment
    vec = { "hello", "not", "luke" };

    // copy construction
    vector<std::string> vec2 = vec;

    // copy assignment
    vector<std::string> vec3; 
    vec3 = vec2;

    // move construction
    vector<std::string> vec4 = std::move(vec3);

    // at this point 'vec3' is currently empty
    
    // move assignment
    vec4 = std::move(vec);

    // at this point 'vec' is currently empty 
  
    // bracket notation
    std::cout << vec4[1] << '\n';

    std::cout << "pVec: " << vec4.begin() << '\n';
    // void resize(size_t newSize)
    vec4.resize(10);
    std::cout << "pVecResize: " << vec4.begin() << '\n';
    for (size_t i = 0; i < vec4.size(); i++) {
        if (vec4[i] == "")
            std::cout << " empty ";
        else
            std::cout << vec4[i];
    }

    // void push_back(const T& value)
    // will add (lvalue or rvalue) to to the end of the list
    // NOTE**: resizing to a larger size constructs objects (usually empty)
    //         so push_back will add to the very end
    vec4.push_back("Hello");
    for (size_t i = 0; i < vec4.size(); i++) {
        if (vec4[i] == "")
            std::cout << " empty ";
        else
            std::cout << vec4[i];
    }

    // let's create a new vector, reserve memory, and then push_back
    vector<std::string> reservedVec;

    // now we reserve 5 unconstructed areas of memory
    reservedVec.reserve(5);
    // push_back will construct in the empty memory and then place value into vector index
    reservedVec.push_back("hello");
    reservedVec.push_back("there");
    // reservedVec now contains only 2 elements
    // void printelm() prints all initialized elements, empty or not
    reservedVec.printelm();

    // T* begin()
    std::string* pVec = reservedVec.begin();

    // T* end()
    std::string* pVecEnd = reservedVec.end();

    for (size_t i = 0; i < 10; i++)
        reservedVec.push_back("hello again");
    
    // void pop_back()
    // destroys memory and decreases size, does not decrease capacity
    reservedVec.pop_back();

    // void clear_resize()
    // clears and brings capacity down to 0
    reservedVec.clear_resize();
    std::cout << "\n\n\n\n";

    // also, this vector works for matrices
    vector<vector<int>> doubleVec;
    vector<int> singleVec = { 1, 2, 3 };
    vector<int> secondVec = { 4, 5, 6 };
    doubleVec.push_back(singleVec);
    doubleVec.push_back(secondVec);

    for (size_t i = 0; i < doubleVec.size(); i++) {
        for (size_t j = 0; j < doubleVec[i].size(); j++) {
            std::cout << doubleVec[i][j] << " ";
        }
        std::cout << '\n';
    }

    std::cout << "\n\n\n\n";
    vector<vector<int>> doubleVec2 = { {1,2,3}, {4,5,6} };
    vector<vector<int>> doubleVec3 = { {7, 8}, {9, 10}, {11, 12} };
    doubleVec2 = doubleVec3;
    for (size_t i = 0; i < doubleVec2.size(); i++) {
        for (size_t j = 0; j < 2; j++)
            std::cout << doubleVec2[i][j] << " ";
        std::cout << '\n';
    }
    return 0;
}




