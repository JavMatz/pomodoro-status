#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

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
    {"--work-minutes",
     [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"-w",
     [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"--rest-minutes",
     [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"-r",
     [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"--long-rest-minutes",
     [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
    {"-l",
     [](TimerSettings &s, const std::string &arg) { s.workTime = stoi(arg); }},
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

void countdown(int minutes) {
  int timerMinutes = minutes;
  int timerSeconds = 0;

  while (timerMinutes >= 0) {
    while (timerSeconds >= 0) {
      std::cout << "\rTime left: " << std::setw(2) << std::setfill('0') << timerMinutes
        << ":" << std::setw(2) << std::setfill('0') << timerSeconds << std::flush;
      std::this_thread::sleep_for(std::chrono::seconds(1));
      --timerSeconds;
    }
    --timerMinutes;
    timerSeconds = 59;
  }
}

std::vector<std::string> split_time_string(const std::string &str,
                                           char delimiter = ':') {
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream ss(str);

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

int main(int argc, const char *argv[]) {

  TimerSettings settings = parse_settings(argc, argv);

  if (settings.help) {
    std::cout << "Usage:" << std::endl;
    std::cout << "\tsimple-pomodoro [OPTIONS] " << std::endl;
    std::cout << "\t\t--help or -h Show this message" << std::endl;
    std::cout << "\t\t--work-minutes or -w Lenght of work session" << std::endl;
    std::cout << "\t\t--rest-minutes or -r Lenght of rest" << std::endl;
    std::cout << "\t\t--long-rest-minutes or -l Lenght of long rest after set of sessions" << std::endl;
    std::cout << "\t\t--sessions or -s Number of sessions before long rest" << std::endl;
    return 0;
  }

  int currentSession = 1;

  while (true) {
    while (currentSession <= settings.sessionsBeforeLongRest) {
      /*std::cout << "Session " << currentSession << std::endl;*/
      /*std::cout << "Working..." << std::endl;*/
      countdown(settings.workTime);
      /*std::cout << "Resting..." << std::endl;*/
      countdown(settings.restTime);

      ++currentSession;
    }
    /*std::cout << "Long Resting..." << std::endl;*/
    countdown(settings.longRestTime);
    currentSession = 1;
  }

  return 0;
}
