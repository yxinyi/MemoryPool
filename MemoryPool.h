#pragma once
template<class T>
class CMemoryPool {
    template<class T>
    struct SNode {
        SNode(T* T_point_, SNode* node_point_) :m_point(T_point_), m_next(node_point_) {};
        T* m_point;
        SNode* m_next;
    };
public:
    CMemoryPool():m_default_size(1000), m_free(nullptr),m_memory_first(nullptr){};
    ~CMemoryPool() {
        //清空
        while(m_memory_first){
            SNode<void>* _del_node = m_memory_first;
            m_memory_first = m_memory_first->m_next;
            operator delete(_del_node);
        }

        while (m_free) {
            SNode<T>* _del_node = m_free;
            m_free = m_free->m_next;
            operator delete(_del_node);
        }

        while (m_null_node) {
            SNode<T>* _del_node = m_null_node;
            m_null_node = m_null_node->m_next;
            operator delete(_del_node);
        }

    }
    void newStack(const size_t size_){
        void* _start_memory = operator new(sizeof T * size_);
        SNode<void>* _node = new SNode<void>(_start_memory, m_memory_first);
        m_memory_first = _node;

        for (size_t _idx = 0; _idx < size_; _idx++) {
            T* _idx_memory = (T*)_start_memory + _idx;
            SNode<T>* _node = new SNode<T>(_idx_memory, m_free);
            m_free = _node;
        }
    }
    T* poolNew() {
        if(!m_free){
            //初始化/扩容
            newStack(m_default_size += m_default_size);
        }

        //从空闲node中拿出
        SNode<T>* _use_node = m_free;
        T* _rst_point = m_free->m_point;
        m_free = m_free->m_next;

        //搜索 placement new 
        T* _rst = new(_rst_point) T();

        _use_node->m_next  = m_null_node;
        _use_node->m_point = nullptr;
        m_null_node = _use_node;
        return _rst;
    }
    bool poolDelete(T* delete_point_) {
        if(!m_null_node){
            return false;
        }

        delete_point_->~T();
        //从空node中拿出
        SNode<T>* _free_node = m_null_node;
        m_null_node = m_null_node->m_next;

        //放入当前空闲内存node
        _free_node->m_point = delete_point_;
        _free_node->m_next = m_free;
        m_free = _free_node;
        return true;
    }
private:
    size_t m_default_size;
    SNode<T>* m_free;
    SNode<T>* m_null_node;
    SNode<void>* m_memory_first;
};


