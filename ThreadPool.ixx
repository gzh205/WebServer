export module ThreadPool;
import <iostream>;
import <vector>;
import <queue>;
import <thread>;
import <mutex>;
import <condition_variable>;
import <functional>;
using namespace std;
export namespace MultiThread {
    class ThreadPool {
    public:
        ThreadPool(size_t num_threads) {
            for (size_t i = 0; i < num_threads; ++i) {
                threads.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            condition_.wait(lock, [this] {
                                return stop_ || !tasks_.empty();
                                });
                            if (stop_ && tasks_.empty()) {
                                return;
                            }
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                    });
            }
        }

        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                stop_ = true;
            }
            condition_.notify_all();
            for (std::thread& thread : threads) {
                thread.join();
            }
        }

        template<typename Func, typename... Args>
        void submit(Func&& func, Args&&... args) {
            std::function<void()> task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            {
                std::unique_lock<std::mutex> lock(mutex_);
                tasks_.emplace(std::move(task));
            }
            condition_.notify_one();
        }

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks_;
        std::mutex mutex_;
        std::condition_variable condition_;
        bool stop_ = false;
    };
}