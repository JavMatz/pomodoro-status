#include <chrono>
#include <csignal>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>

volatile sig_atomic_t stopTimerFlag = 0;

struct TimerSettings {
  bool help{false};
  int workTime{25};
  int restTime{5};
  int longRestTime{15};
  int sessionsBeforeLongRest{4};
};

typedef std::function<void(TimerSettings &)> NoArgHandle;
typedef std::function<void(TimerSettings &, const std::string &)> OneArgHandle;

const std::unordered_map<std::string, NoArgHandle> NoArgs{
    {"--help", [](TimerSettings &s) { s.help = true; }},
    {"-h", [](TimerSettings &s) { s.help = true; }},
};

const std::unordered_map<std::string, OneArgHandle> OneArgs{
    {"--work-minutes", [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"-w", [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"--rest-minutes", [](TimerSettings &s, const std::string &arg) { s.restTime = stoi(arg); }},
    {"-r", [](TimerSettings &s, const std::string &arg) { s.restTime = stoi(arg); }},
    {"--long-rest-minutes", [](TimerSettings &s, const std::string &arg) { s.longRestTime = stoi(arg); }},
    {"-l", [](TimerSettings &s, const std::string &arg) { s.longRestTime = stoi(arg); }},
    {"--sessions", [](TimerSettings &s, const std::string &arg) { s.sessionsBeforeLongRest = stoi(arg); }},
    {"-s", [](TimerSettings &s, const std::string &arg) { s.sessionsBeforeLongRest = stoi(arg); }},
};

TimerSettings parse_settings(int argc, const char *argv[]) {
  TimerSettings settings;

  for (int i{1}; i < argc; i++) {
    std::string opt{argv[i]};

    if (auto j{NoArgs.find(opt)}; j != NoArgs.end()) {
      j->second(settings);
    } else if (auto k{OneArgs.find(opt)}; k != OneArgs.end()) {
      ;
      if (++i < argc) {
        k->second(settings, {argv[i]});
      } else {
        throw std::runtime_error{"missing param after"};
      }
    } else {
      std::cerr << "No option" << std::endl;
    }
  }

  return settings;
}

void skip_this_timer(int signal) {
  if (signal == SIGUSR1) {
    stopTimerFlag = 1;
  }
}

void countdown(int minutes, std::string type) {
  int timerMinutes = minutes;
  int timerSeconds = 0;

  while (!stopTimerFlag) {
    while (timerMinutes >= 0) {
      if (stopTimerFlag) break;
      while (timerSeconds >= 0) {
        if (stopTimerFlag) break;
        std::cout << type
          << "\t" << std::setw(2) << std::setfill('0') << timerMinutes
          << ":" << std::setw(2) << std::setfill('0') << timerSeconds
          << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        --timerSeconds;
      }
      --timerMinutes;
      timerSeconds = 59;
    }
  }
  stopTimerFlag = 0;
  std::cout << std::endl;
}

int main(int argc, const char *argv[]) {

  signal(SIGUSR1, skip_this_timer);

  TimerSettings settings = parse_settings(argc, argv);

  if (settings.help) {
    std::cout << "Usage:" << std::endl;
    std::cout << "\tsimple-pomodoro [OPTIONS] " << std::endl;
    std::cout << "\t\t--help or -h Show this message" << std::endl;
    std::cout << "\t\t--work-length or -w <lenght of work session in minutes> "
                 "(default: 25 min)"
              << std::endl;
    std::cout
        << "\t\t--rest-length or -r <lenght of rest minutes> (default: 5 min)"
        << std::endl;
    std::cout << "\t\t--long-rest-length or -l <lenght of long rest after set "
                 "of sessions minutes> (default: 15 min)"
              << std::endl;
    std::cout << "\t\t--sessions or -s <number of sessions before long rest> "
                 "(default: 4 sessions)"
              << std::endl;
    return 0;
  }

  int currentSession = 1;

  while (true) {
    while (currentSession <= settings.sessionsBeforeLongRest) {
      countdown(settings.workTime, "work");
      countdown(settings.restTime, "rest");

      ++currentSession;
    }
    countdown(settings.longRestTime, "longRest");
    currentSession = 1;
  }

  return 0;
}
