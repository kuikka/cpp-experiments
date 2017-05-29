#include <functional>
#include <mutex>
#include <list>
#include <memory>

class main_loop
{
  public:
    main_loop();
    ~main_loop();

    bool run_on_main_loop(std::function<void(void)> f)
    {
      if (m_stopped)
        return false;

      auto r = make_unique<struct runnable>();

      {
        std::lock_guard<std::mutex> l{m_runnable_lock};
      }

      return true;
    };

  private:
    bool m_stopped = false;
    std::mutex m_runnable_lock;
    struct runnable {
      std::function(void(void)> f;
    };
    std::list<std::unique_ptr<struct runnable> > m_runnables;
};

int main(int /* argc */, char ** /* argv[] */)
{
}
