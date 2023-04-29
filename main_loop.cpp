#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <list>
#include <memory>

class main_loop
{
  public:
    main_loop() = default;
    ~main_loop() = default;

    auto run_on_main_loop(std::function<void(void)> f, const std::chrono::steady_clock::time_point& when) -> bool
    {
      if (m_stopped)
        return false;

      auto r = std::make_unique<struct runnable>();
      r->func = f;
      r->when = when;

      {
        std::lock_guard<std::mutex> l{m_runnable_lock};
	      m_runnables.push_back(std::move(r));
      }

      return true;
    }

    auto run_on_main_loop(std::function<void(void)> f) -> bool
    {
      if (m_stopped)
        return false;

      auto r = std::make_unique<struct runnable>();
      r->func = f;

      {
        std::lock_guard<std::mutex> l{m_runnable_lock};
      	m_runnables.push_back(std::move(r));
      }

      return true;
    };

  private:
    std::atomic<bool> m_stopped = false;
    std::mutex m_runnable_lock;
    struct runnable {
      std::function<void(void)> func;
      std::chrono::steady_clock::time_point when;
    };
    std::list<std::unique_ptr<struct runnable> > m_runnables;
};

auto main(int /* argc */, char ** /* argv[] */) -> int
{
  main_loop m;
}
