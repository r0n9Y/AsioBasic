#pragma once

#include <mutex>
#include <deque>

namespace net
{
    template<typename T>
    class tsqueue {
        public:
        tsqueue() = default;
        tsqueue( const tsqueue& ) = delete;
        virtual ~tsqueue() {clear();}

        public:
        const T& front()
        {
            std::lock_guard<std::mutex>(m_qMutex);
            return m_deQ.front();
        }

        const T& back()
        {
            std::lock_guard<std::mutex>(m_qMutex);
            return m_deQ.back();
        }

        void push_front(const T& data)
        {
            std::lock_guard<std::mutex>(m_qMutex);
            return m_deQ.emplace_front(data);
        }

        void push_back(const T& data)
        {
            std::lock_guard<std::mutex>(m_qMutex);
            m_deQ.emplace_back(data);
        }

        T pop_front()
        {
            std::lock_guard<std::mutex>(m_qMutex);
            T value = std::move(m_deQ.front());
            m_deQ.pop_front();
            return value;
        }

        T pop_back()
        {
            std::lock_guard<std::mutex>(m_qMutex);
            T value = std::move(m_deQ.back());
            m_deQ.pop_back();
            return value;
        }

        bool empty() const
        {
            std::lock_guard<std::mutex>(m_qMutex);
            return m_deQ.empty();
        }

        std::size_t count() const 
        {
            std::lock_guard<std::mutex>(m_qMutex);
            return m_deQ.size();
        }

        void clear()
        {
            std::lock_guard<std::mutex>(m_qMutex);
            m_deQ.clear();
        }
    private:
        std::mutex m_qMutex;
        std::deque<T> m_deQ;
    };
} // namespace net