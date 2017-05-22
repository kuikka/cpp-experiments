#include <atomic>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <thread>
#include <string>
#include <vector>

template <typename T>
class thread_pool
{
public:

  using return_type = T;

  thread_pool(unsigned int threads = std::thread::hardware_concurrency())
    : m_nr_threads(threads)
  {
  }

  ~thread_pool()
  {
    stop();
  }

  void start()
  {
    m_running = true;

    for(int i = 0; i < m_nr_threads; i++)
    {
      m_threads.push_back(std::thread(&thread_pool::thread_runner, this));
    }
  }

  void stop()
  {
    m_running.store(false);
    m_request_cv.notify_all();

    for ( auto& t : m_threads )
      t.join();
  }

  std::future<T> submit(std::function<T(void)> f)
  {
    auto r = std::make_unique<struct request>();
    auto ret = r->p.get_future();
    r->f = f;

    {
      std::lock_guard<std::mutex> l{m_request_lock};
      m_requests.push_back(std::move(r));
    }
    m_request_cv.notify_one();

    return ret;
  }

private:
  void thread_runner()
  {
    while(m_running)
    {
      request_type req;

      {
        std::unique_lock<std::mutex> l{m_request_lock};
        m_request_cv.wait(l, [this]() {return !m_requests.empty() || !m_running;});
        if (!m_running)
          return;

        req = std::move(m_requests.front());
        m_requests.pop_front();
      }

      T ret = req->f();
      req->p.set_value(ret);
    }
  };

private:
  unsigned int m_nr_threads;
  std::vector<std::thread> m_threads;
  struct request {
    std::promise<T> p;
    std::function<T(void)> f;
  };
  using request_type = std::unique_ptr<struct request>;

  std::list<request_type> m_requests;
  std::mutex m_request_lock;
  std::condition_variable m_request_cv;
  std::atomic<bool> m_running;
};

const unsigned rows = 1000000;
const unsigned cols = 256;

static unsigned matrix[cols][rows];

int main(int argc, char *argv[])
{
  // Populate matrix
  srandom(42);
  for (int r = 0; r < rows; ++r) {
    for (int c = 0; c < cols; ++c) {
      matrix[c][r] = random();
    }
  }

  unsigned nr_threads = std::thread::hardware_concurrency();

  if (argc > 1)
    nr_threads = std::stoul(argv[1]);

  using return_type = std::uint64_t;
  thread_pool<return_type> p{ nr_threads };

  auto starttime = std::chrono::steady_clock::now();

  p.start();

  const unsigned row_step = rows / nr_threads;

  std::vector<std::future<return_type> > futures;

  for (unsigned i = 0, start_row = 0; i < nr_threads; i++, start_row += row_step) {
    std::future<return_type> f = p.submit([start_row, row_step]() -> return_type {
        return_type v = 0;
        for (int r = start_row; r < start_row + row_step; r++) {
          for (int c = 0; c < cols; ++c) {
            v += matrix[c][r];
          }
        }
        return v;
        });
    futures.push_back(std::move(f));
  }

  return_type total = 0;
  for ( auto& f : futures )
  {
    total += f.get();
  }

  auto endtime = std::chrono::steady_clock::now();
  auto int_ms = std::chrono::duration_cast<std::chrono::microseconds>(endtime - starttime);

  std::cout << "Time " << int_ms.count() << " ms\n";
  std::cout << "Total " << total << '\n';

}
