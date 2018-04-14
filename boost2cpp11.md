# 将Muduo库中使用boost的地方替换为c++11的新特性
```c++
1 boost::noncopyable -> rawmd::noncopyable(rawmd/base/noncopyable.h)
2 BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t)) -> static_assert(sizeof(Timestamp) == sizeof(int64_t),
              "Timestamp should be same size as int64_t");
3 boost::ptr_vector<T>push_back(new T) -> std::vector<std::unique_ptr<T>>::push_bck(static_cast<std::unique_ptr<T>(new T));
4 BOOST_TEST -> Minigtest
5 boost::scoped_ptr -> std::unique_ptr    *Pls attention that unique_ptr does not have unique_ptr::release method，its inner class object will live as long as the program.*
```