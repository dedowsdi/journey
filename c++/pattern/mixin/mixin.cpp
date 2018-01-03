/*
 * A Mixin is a class that contains methods for use by other classes without
 * having to be the parent class of those other classes.  Mixins are sometimes
 * described as being "included" rather than "inherited".
 */

#include <iostream>

struct ITask {
  virtual ~ITask(){};
  virtual std::string getName() = 0;
  virtual void execute() = 0;
};

// mixin 1. This class isn't child of ITask, which means it doesn't have to
// implement ITask::getName of stuff like that, it can focus on what it must do.
template <class T>
class LoggingTask : public T {
public:
  void execute() {
    std::cout << "LOG: The task is starting - " << T::getName() << std::endl;
    T::execute();
    std::cout << "LOG: The task has completed - " << T::getName() << std::endl;
  }
};

// mixin 2. This class isn't child of ITask, which means it doesn't have to
// implement ITask::getName of stuff like that, it can focus on what it must do.
template <class T>
class TimingTask : public T {
public:
  void execute() {
    T::execute();
    std::cout << "Task Duration: xxxx seconds" << std::endl;
  }
};

// mixin user. This class implements ITask, if you want you can make it child of
// ITask, it doesn't matter, you will need a adapter anyway.
class MyTask {
public:
  void execute() {
    std::cout << "...This is where the task is executed..." << std::endl;
  }

  std::string getName() { return "My task name"; }
};

// I think this adapter is acutally a special kind of mixin, it has the ability
// to adapte T to ITask.
template <class T>
class TaskAdapter : public ITask, public T {
public:
  virtual void execute() { T::execute(); }

  virtual std::string getName() { return T::getName(); }
};

int main(int argc, char* argv[]) {
  // A plain old MyTask
  MyTask t1;
  t1.execute();
  std::cout << std::endl;

  // A MyTask with added timing:
  TimingTask<MyTask> t2;
  t2.execute();
  std::cout << std::endl;

  // A MyTask with added logging and timing:
  LoggingTask<TimingTask<MyTask>> t3;
  t3.execute();
  std::cout << std::endl;

  // A MyTask with added logging and timing written to look a bit like the
  // composition example
  typedef LoggingTask<TimingTask<MyTask>> Task;
  Task t4;
  t4.execute();
  std::cout << std::endl;

  // typedef for our final class, inlcuding the TaskAdapter<> mixin
  typedef TaskAdapter<LoggingTask<TimingTask<MyTask>>> task;

  // instance of our task - note that we are not forced into any heap
  // allocations!
  task t;

  // implicit conversion to ITask* thanks to the TaskAdapter<>
  ITask* it = &t;
  it->execute();

  return 0;
}
