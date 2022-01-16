#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <optional>
#include <queue>
#include <mutex>

template<typename T>
class ThreadsafeQueue 
{
   std::queue<T> queue_;
   mutable std::mutex mutex_;
   
   // Moved out of public interface to prevent races between this
   // and pop().
   bool empty() const 
   {
      return queue_.empty();
   }
   
   public:
   ThreadsafeQueue() = default;
   ThreadsafeQueue(const ThreadsafeQueue<T> &) = delete ;
   ThreadsafeQueue& operator=(const ThreadsafeQueue<T> &) = delete ;
   
   ThreadsafeQueue(ThreadsafeQueue<T>&& other) 
   {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_ = std::move(other.queue_);
   }
   
   virtual ~ThreadsafeQueue() { }
   
   unsigned long size() const 
   {
      std::lock_guard<std::mutex> lock(mutex_);
      return queue_.size();
   }
   
   T pop() 
   {
      std::lock_guard<std::mutex> lock(mutex_);
      T tmp;
      if (! queue_.empty()) 
      
      {
         tmp = queue_.front();
         queue_.pop();
      }
      return tmp;
   }
   
   void push(const T &item) 
   {
      std::lock_guard<std::mutex> lock(mutex_);
      queue_.push(item);
   }
};

#endif
