# MemoryPool

# 代码为定长内存池

预先分配一块大内存,后续再在该块内存上进行申请分配

node为承载具体对象指针的句柄类,使用单向链表链接,但其实更像栈一点,会在一端进行操作

因此在具体创建和销毁时,效率为O(1),再分配节点时效率为O(N)

当一块大内存使用完后,也就是当m_free == nullptr 时,会再分配一块大内存,大小为初始值的一倍

内存池没有考虑大内存块的销毁,既然进行了一次扩容,那后续也会继续扩展到该大小,因分配节点效率较低,可以视为用空间换时间


使用方法:

    CMemoryPool<T> _pool;
    
获取元素:

    T* _obj = _pool.poolNew();

销毁元素

    _pool.poolDelete(_obj);
    
    
![](https://github.com/yxinyi/MemoryPool/blob/master/pic/memory.png?raw=true)
  




