/******************************************************************************************
 * Read And Write Muduo(RAWMD)
 * wzhe, ahuwang@163.com
 ******************************************************************************************/

#pragma once


namespace rawmd
{
/// A tag class emphasises the objects are  noncopyable.
/// Any derived class of noncopyable should be a object type.
class noncopyable
{
protected:
    noncopyable() = default;
    ~noncopyable() = default;

private:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
};

}
