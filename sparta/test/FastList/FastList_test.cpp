
#include "sparta/utils/FastList.hpp"
#include "sparta/utils/SpartaTester.hpp"

#include <list>
#include <chrono>


uint32_t my_obj_deletions = 0;
class MyObj
{
public:
    explicit MyObj(uint32_t v) : v_(v) {}

    MyObj(const MyObj &) = delete;

    ~MyObj() { ++my_obj_deletions; }

    uint32_t getV() const { return v_; }

    bool operator==(const uint32_t in) const {
        return in == v_;
    }
private:
    const uint32_t v_;
};

void testConst(const sparta::utils::FastList<MyObj> & fl)
{
    size_t i = 9;
    for(auto & val : fl) {
        EXPECT_EQUAL(val, i);
        --i;
    }
}

std::ostream & operator<<(std::ostream & os, const MyObj & obj) {
    os << "MyObj(" << obj.getV() << ")";
    return os;
}

void testFastList()
{
    sparta::utils::FastList<MyObj> fl(10);
    std::cout << fl;

    my_obj_deletions = 0;

    ////////////////////////////////////////
    // emplace_front
    auto itr = fl.emplace_front(0);
    std::cout << "Added one: \n" << fl;
    EXPECT_TRUE(*itr == 0);

    auto itr1 = fl.emplace_front(1);
    std::cout << "Added another: \n" << fl;
    EXPECT_TRUE(*itr1 == 1);

    auto itr2 = fl.emplace_front(2);
    std::cout << "Added another: \n" << fl;
    EXPECT_TRUE(*itr2 == 2);
    EXPECT_TRUE(fl.size() == 3);
    EXPECT_TRUE(fl.max_size() == 10);

    // Nothing should have been deleted
    EXPECT_EQUAL(my_obj_deletions, 0);

    ////////////////////////////////////////
    // erase
    auto next_it = fl.erase(itr1);
    std::cout << "Erased: \n" << fl;
    EXPECT_TRUE(fl.size() == 2);
    // push front 2, 1, 0.  Remove 1, next is 0
    EXPECT_EQUAL(next_it->getV(), 0);

    fl.erase(itr2);
    std::cout << "Erased: \n" << fl;
    EXPECT_TRUE(fl.size() == 1);

    next_it = fl.erase(itr);
    std::cout << "Erased: \n" << fl;
    EXPECT_TRUE(fl.size() == 0);
    EXPECT_FALSE(next_it.isValid());

    // Should have 3 objects deleted
    EXPECT_EQUAL(my_obj_deletions, 3);

    //EXPECT_THROW(fl.erase(itr));
    my_obj_deletions = 0;

    const size_t num_elems = fl.max_size();
    for(size_t i = 0; i < num_elems; ++i) {
        fl.emplace_front(i);
    }
    // go beyond the end of the array and expect crash
    EXPECT_THROW(fl.emplace_front(100));

    size_t i = 9;
    for(const auto & val : fl) {
        EXPECT_EQUAL(val, i);
        --i;
    }
    testConst(fl);
    EXPECT_EQUAL(my_obj_deletions, 0);

    ////////////////////////////////////////////////////////////
    // Clear
    fl.clear();
    EXPECT_EQUAL(my_obj_deletions, 10);
    EXPECT_TRUE(fl.empty());
    EXPECT_EQUAL(fl.size(), 0);
    EXPECT_TRUE(fl.begin() == fl.end());

    ////////////////////////////////////////////////////////////
    // pop_back
    my_obj_deletions = 0;
    for(size_t i = 0; i < num_elems; ++i) {
        fl.emplace_front(i);
    }
    EXPECT_EQUAL(my_obj_deletions, 0);
    EXPECT_EQUAL(fl.size(), 10);
    fl.pop_back();  // Remove element 0
    EXPECT_EQUAL(fl.size(), 9);
    i = 9;
    for(const auto & val : fl) {
        EXPECT_EQUAL(val, i);
        --i;
    }
    auto sz = fl.size();
    for(size_t i = 0; i < sz; ++i) {
        fl.pop_back();
    }
    EXPECT_EQUAL(fl.size(), 0);
    EXPECT_THROW(fl.pop_back());
    EXPECT_EQUAL(my_obj_deletions, 10);

    ////////////////////////////////////////////////////////////
    // pop_back
    my_obj_deletions = 0;
    for(size_t i = 0; i < num_elems; ++i) {
        fl.emplace_front(i);
    }
    EXPECT_EQUAL(my_obj_deletions, 0);
    EXPECT_EQUAL(fl.size(), 10);
    fl.pop_front();  // Remove element 9
    EXPECT_EQUAL(fl.size(), 9);
    i = 8;
    for(const auto & val : fl) {
        EXPECT_EQUAL(val, i);
        --i;
    }
    for(size_t i = 0; i < sz; ++i) {
        fl.pop_front();
    }
    EXPECT_EQUAL(fl.size(), 0);
    EXPECT_THROW(fl.pop_front());
    EXPECT_EQUAL(my_obj_deletions, 10);

    ////////////////////////////////////////////////////////////
    // emplace
    my_obj_deletions = 0;
    fl.clear();
    EXPECT_EQUAL(fl.size(), 0);
    auto empl_it = fl.emplace(fl.begin(), 10);
    EXPECT_EQUAL(empl_it->getV(), 10);
    EXPECT_EQUAL(fl.size(), 1);
    empl_it = fl.begin();
    EXPECT_EQUAL(empl_it->getV(), 10);
    empl_it = fl.emplace(empl_it, 20);
    EXPECT_EQUAL(empl_it->getV(), 20);
    ++empl_it;
    EXPECT_EQUAL(empl_it->getV(), 10);

    fl.clear();
    EXPECT_EQUAL(my_obj_deletions, 2);
    my_obj_deletions = 0;
    auto empl_it_30 = fl.emplace(fl.end(), 30);
    EXPECT_EQUAL(empl_it->getV(), 30);
    empl_it = fl.emplace(fl.begin(), 40);
    EXPECT_EQUAL(empl_it->getV(), 40);
    empl_it = fl.emplace(empl_it_30, 50);
    EXPECT_EQUAL(fl.size(), 3);

    // Should have 40, 50, 30
    empl_it = fl.begin();
    EXPECT_EQUAL(empl_it->getV(), 40);
    ++empl_it;
    EXPECT_EQUAL(empl_it->getV(), 50);
    ++empl_it;
    EXPECT_EQUAL(empl_it->getV(), 30);
    fl.clear();
    EXPECT_EQUAL(my_obj_deletions, 3);

    my_obj_deletions = 0;
    {
        sparta::utils::FastList<MyObj> fl2(10);
        for(size_t i = 0; i < num_elems; ++i) {
            fl2.emplace_front(i);
        }
    }
    EXPECT_EQUAL(my_obj_deletions, 10);

}

#define PERF_TEST 100000000
template<class ListType>
void testListPerf()
{
    ListType fl(10);
    const int num_elems = 10;
    for(int i = 0; i < PERF_TEST; ++i) {
        for(size_t i = 0; i < num_elems; ++i) {
            fl.emplace_front(i);
        }

        const auto end = fl.end();
        for(auto it = fl.begin(); it != end;) {
            fl.erase(it++);
        }
    }
}

int main()
{
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    std::cout.precision(12);

    testFastList();

    auto start = std::chrono::system_clock::system_clock::now();
    testListPerf<sparta::utils::FastList<int>>();
    auto end = std::chrono::system_clock::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Raw time (seconds) fast list : " << dur / 1000000.0 << std::endl;

    start = std::chrono::system_clock::system_clock::now();
    testListPerf<std::list<int>>();
    end = std::chrono::system_clock::system_clock::now();
    dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Raw time (seconds) old list : " << dur / 1000000.0 << std::endl;

    // Done
    REPORT_ERROR;

    return ERROR_CODE;
}
